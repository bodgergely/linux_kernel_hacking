# .ycm_extra_conf.py for kernel
import os

# Attention:
# File path not starting with / or = will be expanded.

flags_c = [
    '-Wall',
    '-Wundef',
    '-Wstrict-prototypes',
    '-Wno-trigraphs',
    '-fno-strict-aliasing',
    '-fno-common',
    '-Werror-implicit-function-declaration',
    '-Wno-format-security',
    '-D__KERNEL__',
    '-DMODULE',
    '-x', 'c',
    '-std=gnu89',
    '-nostdinc',
    '--sysroot=/home/robertma/sfd-kernel',  # workspace
    # Not sure if sysroot works in clang
    '-isystem', '/home/robertma/sfd-kernel/usr/local/include',
    '-isystem', '/home/robertma/sfd-kernel/usr/include',
    '-isystem', '/home/robertma/sfd-kernel/usr/lib/gcc/i686-redhat-linux/4.5.1/include',
    # Will be path mangled
    '-I', 'include',
    '-I', 'arch/x86/include',
    '-I', 'arch/x86/include/generated',
    '-I', 'bld-x86_64/include',
    '-I', 'bld-x86_64/arch/x86/include',
    '-I', 'bld-x86_64/arch/x86/include/generated',
    '-include', 'include/linux/kconfig.h',  # IMPORTANT
]


def DirectoryOfThisScript():
    return os.path.dirname(os.path.abspath(__file__))


def MakeRelativePathsInFlagsAbsolute(flags, working_directory):
    if not working_directory:
        return flags
    new_flags = []
    make_next_absolute = False
    path_flags = ['-isystem', '-I', '-iquote', '--sysroot=', '-include']
    for flag in flags:
        new_flag = flag

        if make_next_absolute:
            make_next_absolute = False
            if not flag.startswith('/') and not flag.startswith('='):
                new_flag = os.path.join(working_directory, flag)

        for path_flag in path_flags:
            if flag == path_flag:
                make_next_absolute = True
                break
            if flag.startswith(path_flag):
                path = flag[len(path_flag):]
                new_flag = path_flag + os.path.join(working_directory, path)
                break

        if new_flag:
            new_flags.append(new_flag)

    return new_flags


def FlagsForFile(filename):
    extension = os.path.splitext(filename)[1]
    if extension == '.cpp':
        assert False
    flags = flags_c

    relative_to = DirectoryOfThisScript()
    final_flags = MakeRelativePathsInFlagsAbsolute(flags, relative_to)
    return {
        'flags': final_flags,
        'do_cache': True
    }
