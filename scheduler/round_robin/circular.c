#include <linux/sched.h>
#include <linux/string.h>
#include <linux/ratelimit.h>

#include "sched.h"


const struct sched_class circular_sched_class;

#define for_each_sched_entity(se) \
		for (; se; se = NULL)

static inline struct task_struct *task_of(struct sched_entity *se)
{
#ifdef CONFIG_SCHED_DEBUG
	WARN_ON_ONCE(!entity_is_task(se));
#endif
	return container_of(se, struct task_struct, se);
}

static inline struct circ_rq *circ_rq_of(struct sched_entity *se)
{
	struct task_struct *p = task_of(se);
	struct rq *rq = task_rq(p);

	return &rq->circ;
}

static inline struct circ_rq *task_circ_rq(struct task_struct *p)
{
	return &task_rq(p)->circ;
}

static struct sched_entity *
pick_next_entity(struct circ_rq *circ_rq, struct sched_entity *curr)
{
	struct list_head *next = circ_rq->circList.head;
	/*
	if(rq->circList.last_chosen)
		next = rq->circList.last_chosen->next;
	else
		next = rq->circList.head->next;

	rq->circList.last_chosen = next;
	*/
	if (!next)
			return NULL;

	return circ_entry(next, struct sched_entity, run_circular_node);
}



static void __enqueue_entity(struct circ_rq *circ_rq, struct sched_entity *se)
{
	struct list_head* link = circ_rq->circList.head;

	//circ_entry(link, struct sched_entity, run_circular_node);

	circularlist_insert(&circ_rq->circList, link, &se->run_circular_node);
}

static void put_prev_entity(struct circ_rq *circ_rq, struct sched_entity *prev)
{
	if (prev->on_rq) {
		/* Put 'current' back into the circle. */
		__enqueue_entity(circ_rq, prev);
	}
	circ_rq->curr = NULL;
}

static void put_prev_task_circular(struct rq *rq, struct task_struct *prev)
{

	struct sched_entity *se = &prev->se;
	struct circ_rq *circ_rq = circ_rq_of(se);
	////printk_ratelimited(KERN_ERR "Before put_prev_task_circular: prev: %p\n", &prev->se);
	//circularlist_traverse_debug(&rq->circList);
	put_prev_entity(circ_rq, se);
	////printk_ratelimited(KERN_ERR "After put_prev_task_circular");
	//circularlist_traverse_debug(&rq->circList);
}



static void
enqueue_entity(struct circ_rq *circ_rq, struct sched_entity *se, int flags)
{
	circ_rq->nr_running++;
	if (se != circ_rq->curr)
			__enqueue_entity(circ_rq, se);
	se->on_rq = 1;
}


static void
enqueue_task_circular(struct rq *rq, struct task_struct *p, int flags)
{
	struct circ_rq *circ_rq;
	struct sched_entity *se = &p->se;

	// DEBUG
	struct list_head* head;
	struct task_struct* task;
	//printk_ratelimited(KERN_ERR "Enqueue task: %p name: %s\n", p, p->comm);
	//printk_ratelimited(KERN_ERR "Before enqueue: rq->circ.circList.count: %d\n", rq->circ.circList.count);
	//printk_ratelimited(KERN_ERR "sched entity to be enqueued: %p\n", se);
	circularlist_traverse_debug(&rq->circ.circList);
	task = task_of(&(rq->curr->se));
	////printk_ratelimited(KERN_ERR "Current task name: %s\n", task->comm);
	// END DEBUG


	for_each_sched_entity(se) {
		if (!se->on_rq)
		{
			circ_rq = circ_rq_of(se);
			enqueue_entity(circ_rq, se, flags);
			circ_rq->h_nr_running++;
			flags = ENQUEUE_WAKEUP;
		}
	}

	if(!se)
		add_nr_running(rq, 1);

	//printk_ratelimited(KERN_ERR "After enqueue: rq->circList.count: %d\n", rq->circ.circList.count);
	circularlist_traverse_debug(&rq->circ.circList);

}

static void __dequeue_entity(struct circ_rq *rq, struct sched_entity *se)
{
	//struct list_head* link = rq->circ->list->head;
	//struct sched_entity *entry;

	//circ_entry(link, struct sched_entity, run_circular_node);

	circularlist_remove(&rq->circList, &se->run_circular_node);
}

static void
dequeue_entity(struct circ_rq *circ_rq, struct sched_entity *se, int flags)
{
	if (se != circ_rq->curr)
		__dequeue_entity(circ_rq, se);
	se->on_rq = 0;
	circ_rq->nr_running--;
}

static void dequeue_task_circular(struct rq *rq, struct task_struct *p, int flags)
{
	struct circ_rq *circ_rq;
	struct sched_entity *se = &p->se;
	//printk_ratelimited(KERN_ERR "Dequeue task: %p name: %s\n", p, p->comm);
	//printk_ratelimited(KERN_ERR "Before dequeue: rq->circList.count: %d\n", rq->circ.circList.count);
	//printk_ratelimited(KERN_ERR "sched entity to be dequeued: %p\n", se);
	circularlist_traverse_debug(&rq->circ.circList);
	for_each_sched_entity(se) {
		circ_rq = circ_rq_of(se);
		dequeue_entity(circ_rq, se, flags);
		circ_rq->h_nr_running--;
	}

	if(!se)
		sub_nr_running(rq, 1);
	//printk_ratelimited(KERN_ERR "After dequeue: rq->circList.count: %d\n", rq->circ.circList.count);
	circularlist_traverse_debug(&rq->circ.circList);
}

static void update_curr_circular(struct rq *rq)
{
	//GERI
	int i;
	//
	struct task_struct *curtask;
	struct sched_entity *curr = &rq->curr->se;
	u64 now = rq_clock_task(rq);
	u64 delta_exec;
	u64 exec_start;

	if (unlikely(!curr))
		return;

	exec_start = curr->exec_start;
	delta_exec = now - curr->exec_start;
	if (unlikely((s64)delta_exec <= 0))
		return;

	curr->exec_start = now;

	schedstat_set(curr->statistics.exec_max,
			  max(delta_exec, curr->statistics.exec_max));

	curr->sum_exec_runtime += delta_exec;

	//curr->vruntime += calc_delta_fair(delta_exec, curr);


	curtask = task_of(curr);

	// GERI
	/*
	if(curtask && curr->cfs_rq)
	{
		if(!strcmp(curtask->comm, "firefox"))
		{
			//printk(KERN_ERR "update_curr: name: %s now: %lu delta_exec: %lu curr->sum_exec_runtime: %lu exec_start:%lu exec_max: %lu curr->vruntime: %lu\n",
					curtask->comm, now, delta_exec,curr->sum_exec_runtime, exec_start,
					curr->statistics.exec_max, curr->vruntime);
		}
	}
	*/
	////printk_ratelimited(KERN_DEBUG "Before trace_sched_stat_runtime");
	//trace_sched_stat_runtime(curtask, delta_exec, curr->vruntime);
	cpuacct_charge(curtask, delta_exec);
	account_group_exec_runtime(curtask, delta_exec);


}

static void check_preempt_wakeup(struct rq *rq, struct task_struct *p, int wake_flags)
{
	struct task_struct *curr = rq->curr;
	struct sched_entity *se = &curr->se;
	struct sched_entity	*pse = &p->se;
	struct circ_rq *circ_rq = task_circ_rq(curr);

	if (unlikely(se == pse))
			return;

	/* Idle tasks are by definition preempted by non-idle tasks. */
	if (unlikely(curr->policy == SCHED_IDLE) &&
		likely(p->policy != SCHED_IDLE))
		goto preempt;

	/*
		 * Batch and idle tasks do not preempt non-idle tasks (their preemption
		 * is driven by the tick):
		 */
	if (unlikely(p->policy != SCHED_NORMAL) || !sched_feat(WAKEUP_PREEMPTION))
		return;

	update_curr_circular(rq);

	#ifdef CONFIG_SMP
		/*
		 * If:
		 *
		 * - the newly woken task is of equal priority to the current task
		 * - the newly woken task is non-migratable while current is migratable
		 * - current will be preempted on the next reschedule
		 *
		 * we should check to see if current can readily move to a different
		 * cpu.  If so, we will reschedule to allow the push logic to try
		 * to move current somewhere else, making room for our non-migratable
		 * task.
		 */
		if (p->prio == rq->curr->prio && !test_tsk_need_resched(rq->curr))
			check_preempt_equal_prio(rq, p);
	#endif


	preempt:
		resched_curr(rq);

}

static void
set_next_entity(struct circ_rq *circ_rq, struct sched_entity *se)
{
	/* 'current' is not kept within the circular list. */
	if (se->on_rq) {
		__dequeue_entity(circ_rq,se);
	}
	circ_rq->curr = se;

}

static void set_curr_task_circular(struct rq *rq)
{
	struct sched_entity *se = &rq->curr->se;
	for_each_sched_entity(se) {
			struct circ_rq *circ_rq = circ_rq_of(se);
			set_next_entity(circ_rq, se);
	}
}

static struct task_struct *
pick_next_task_circular(struct rq *rq, struct task_struct *prev)
{
	struct circ_rq *circ_rq = &rq->circ;
	struct sched_entity *se;
	struct task_struct *p;
	int new_tasks;
	// DEBUG
	/*
	struct list_head* node;
	struct task_struct* task;
	//printk_ratelimited(KERN_ERR "rq->curr pointer: %p name: %s\n", rq->curr, rq->curr->comm);
	//printk_ratelimited(KERN_ERR "rq->circList.count: %d\n", rq->circList.count);
	task = task_of(&(rq->curr->se));
	//printk_ratelimited(KERN_ERR "Current task name: %s\n", task->comm);
	se = circ_entry(rq->curr->se.run_circular_node.next, struct sched_entity, run_circular_node);
	task = task_of(se);
	//printk_ratelimited(KERN_ERR "Next task name: %s\n", task->comm);
	node = rq->circList.head;
	do{
		//printk_ratelimited(KERN_ERR "se->run_circular_node: %p\n", node);
		se = circ_entry(node, struct sched_entity, run_circular_node);
		task = task_of(se);
		//printk_ratelimited(KERN_ERR "Task name: %s\n", task->comm);
		node = se->run_circular_node.next;
	}while(node!=rq->circList.head);
	*/
	////printk_ratelimited(KERN_ERR "Before pick_next_task_circular: curr: %p  prev: %p\n", &(rq->curr->se), &(prev->se));
	//circularlist_traverse_debug(&rq->circList);
	// END OF DEBUG
	// tegyuk vissza a circlistaba az "elozot" ami meg eppen most a current de a previous lesz emiatt visszakerul a runqeueue-na
	if (!circ_rq->nr_running)
			goto idle;

	put_prev_task(rq, prev);
	se = pick_next_entity(circ_rq, NULL);
	if(!se)
		se = circ_rq->curr;
	else
		set_next_entity(circ_rq, se);
	//printk_ratelimited(KERN_ERR "Pick next task: %p -> %p Run queue: %d\n", prev, task_of(se), circ_rq->circList.count);
	//printk_ratelimited(KERN_ERR "Pick next task: %s -> %s Run queue: %d\n", prev->comm, task_of(se)->comm, circ_rq->circList.count);
	////printk_ratelimited(KERN_ERR "After pick_next_task_circular: curr: %p  prev: %p\n", &(rq->curr->se), &(prev->se));
	circularlist_traverse_debug(&circ_rq->circList);

	p = task_of(se);
	return p;
idle:

	return NULL;
}



static void yield_task_circular(struct rq *rq)
{
	struct task_struct *curr = rq->curr;
	struct sched_entity *se = &curr->se;

	/*
	 * Are we the only task in the tree?
	 */
	if (unlikely(rq->nr_running == 1))
		return;
	//printk(KERN_ERR "yield_to_task_circular: task that yields: %s\n", rq->curr->comm);

	if (curr->policy != SCHED_BATCH) {
		update_rq_clock(rq);
		/*
		 * Update run-time statistics of the 'current'.
		 */
		update_curr_circular(rq);
		/*
		 * Tell update_rq_clock() that we've just updated,
		 * so we don't do microscopic update in schedule()
		 * and double the fastpath cost.
		 */
		 rq->skip_clock_update = 1;
	}


}

static bool yield_to_task_circular(struct rq *rq, struct task_struct *p, bool preempt)
{
	//printk_ratelimited(KERN_DEBUG "yield_to_task_circular(struct rq *rq)");
	struct sched_entity *se = &p->se;
	if (!se->on_rq)
		return false;
	//printk(KERN_ERR "yield_to_task_circular: curr: %s -> se: %s\n", rq->curr->comm, p->comm);
	yield_task_circular(rq);
	return true;
}

static void task_tick_circular(struct rq *rq, struct task_struct *p, int queued)
{
	//printk_ratelimited(KERN_DEBUG "task_tick_circular(struct rq *rq, struct task_struct *curr, int queued)");
	struct sched_entity *se = &p->se;
	struct circ_rq *circ_rq = circ_rq_of(se);

	//update_curr_circular(rq);

	/*
	 * Requeue to the end of queue if we (and all of our ancestors) are not
	 * the only element on the queue
	 */

	/*
	if (circ_rq->nr_running > 1) {
		//printk_ratelimited(KERN_ERR "task_tick_circular rescheduling: %s\n", p->comm);
		//requeue_task_rt(rq, p, 0); MIGHT NEED TO IMPLEMENT!!!
		resched_curr(rq);
		return;
	}
	*/
}

static void task_fork_circular(struct task_struct *p)
{
	//printk_ratelimited(KERN_DEBUG "task_fork_circular(struct task_struct *p)");
	struct sched_entity *se = &p->se, *curr;
	int this_cpu = smp_processor_id();
	struct rq *rq = this_rq();
	unsigned long flags;

	raw_spin_lock_irqsave(&rq->lock, flags);

	update_rq_clock(rq);

	curr = &rq->curr->se;

	/*
	 * Not only the cpu but also the task_group of the parent might have
	 * been changed after parent->se.parent,cfs_rq were copied to
	 * child->se.parent,cfs_rq. So call __set_task_cpu() to make those
	 * of child point to valid ones.
	 */
	rcu_read_lock();
	__set_task_cpu(p, this_cpu);
	rcu_read_unlock();

	//update_curr(rq);

	//if (curr)
	//	se->vruntime = curr->vruntime;
	//place_entity(cfs_rq, se, 1);

	// this might be fucked up here
	if (sysctl_sched_child_runs_first && curr /*&& entity_before(curr, se)*/) {
		/*
		 * Upon rescheduling, sched_class::put_prev_task() will place
		 * 'current' within the tree based on its new key value.
		 */
		//printk_ratelimited(KERN_ERR "task_for_circular swapping: %s to %s\n", task_of(curr)->comm,task_of(se)->comm);
		swap(curr->vruntime, se->vruntime);
		resched_curr(rq);
	}

	//se->vruntime -= cfs_rq->min_vruntime;

	raw_spin_unlock_irqrestore(&rq->lock, flags);
}

static void
prio_changed_circular(struct rq *rq, struct task_struct *p, int oldprio)
{
	printk_ratelimited(KERN_ERR "prio_changed_circular: %s oldprio: %d \n", p->comm, oldprio);
	if (rq->curr == p) {
		if (p->prio > oldprio)
		{
			//printk_ratelimited(KERN_ERR "prio_changed_circular rescheduling: %s oldprio: %d \n", p->comm, oldprio);
			resched_curr(rq);
		}
	} else
		check_preempt_curr(rq, p, 0);
}

static void switched_from_circular(struct rq *rq, struct task_struct *p)
{
	//printk_ratelimited(KERN_DEBUG "switched_from_circular(struct rq *rq, struct task_struct *p)");
}

static void switched_to_circular(struct rq *rq, struct task_struct *p)
{
	//printk_ratelimited(KERN_ERR "switched_to_circular: %s\n", p->comm);
	/*
		 * We were most likely switched from sched_rt, so
		 * kick off the schedule if running, otherwise just see
		 * if we can still preempt the current task.
		 */

	if (!task_on_rq_queued(p))
			return;

	if (rq->curr == p)
	{
		//printk_ratelimited(KERN_ERR "switched_to_circular rescheduling: %s\n", p->comm);
		resched_curr(rq);
	}
	else
		check_preempt_curr(rq, p, 0);
}

static unsigned int get_rr_interval_circular(struct rq *rq, struct task_struct *task)
{
	//printk_ratelimited(KERN_DEBUG "get_rr_interval_circular(struct rq *rq, struct task_struct *task)");
}

/*
 * All the scheduling class methods:
 */
const struct sched_class circular_sched_class = {
	.next			= &idle_sched_class,
	.enqueue_task		= enqueue_task_circular,
	.dequeue_task		= dequeue_task_circular,
	.yield_task		= yield_task_circular,
	.yield_to_task		= yield_to_task_circular,

	.check_preempt_curr	= check_preempt_wakeup,

	.pick_next_task		= pick_next_task_circular,
	.put_prev_task		= put_prev_task_circular,

	/*
#ifdef CONFIG_SMP
	.select_task_rq		= select_task_rq_circular,
	.migrate_task_rq	= migrate_task_rq_circular,

	.rq_online		= rq_online_circular,
	.rq_offline		= rq_offline_circular,

	.task_waking		= task_waking_circular,
#endif
*/
	.set_curr_task          = set_curr_task_circular,
	.task_tick		= task_tick_circular,
	.task_fork		= task_fork_circular,

	.prio_changed		= prio_changed_circular,
	.switched_from		= switched_from_circular,
	.switched_to		= switched_to_circular,

	.get_rr_interval	= get_rr_interval_circular,

	.update_curr		= update_curr_circular
/*
#ifdef CONFIG_FAIR_GROUP_SCHED
	.task_move_group	= task_move_group_fair,
#endif
*/
};
