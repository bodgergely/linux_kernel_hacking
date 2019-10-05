
SRC=$@

[ "$SRC" = "" ] && { SRC=`q files`; }

FILE=/tmp/1.c

echo "                                      errors   lines of code   errors/KLOC"

for DIR in $SRC; do

  printf "%-35s " $DIR
  find $DIR -name '*.[ch]' | xargs cat > $FILE
  RES=`scripts/checkpatch.pl --file $FILE | grep ^total`

  # echo $RES

  if [ "$RES" != "" ]; then
    ERR=`echo $RES | cut -d' ' -f2`
    WARN=`echo $RES | cut -d' ' -f4`
    LINES=`echo $RES | cut -d' ' -f6`
  else
    ERR=0
    WARN=0
    LINES=`cat $FILE | wc -l`
  fi

  LINES=$[$LINES+1]
  rm -f $FILE

  ERR_KLOC=`( echo "scale=1"; echo "$ERR*1000/$LINES" ) | bc`
  printf "%8s      %10s    %10s\n" $ERR $LINES $ERR_KLOC

done

