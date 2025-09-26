#
# Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008 The SCons Foundation
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
# KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

__doc__ = """
SCons compatibility package for old Python versions

This subpackage holds modules that provide backwards-compatible
implementations of various things that we'd like to use in SCons but which
only show up in later versions of Python than the early, old version(s)
we still support.

Other code will not generally reference things in this package through
the SCons.compat namespace.  The modules included here add things to
the __builtin__ namespace or the global module list so that the rest
of our code can use the objects and names imported here regardless of
Python version.

Simply enough, things that go in the __builtin__ name space come from
our builtins module.

The rest of the things here will be in individual compatibility modules
that are either: 1) suitably modified copies of the future modules that
we want to use; or 2) backwards compatible re-implementations of the
specific portions of a future module's API that we want to use.

GENERAL WARNINGS:  Implementations of functions in the SCons.compat
modules are *NOT* guaranteed to be fully compliant with these functions in
later versions of Python.  We are only concerned with adding functionality
that we actually use in SCons, so be wary if you lift this code for
other uses.  (That said, making these more nearly the same as later,
official versions is still a desirable goal, we just don't need to be
obsessive about it.)

We name the compatibility modules with an initial '_scons_' (for example,
_scons_subprocess.py is our compatibility module for subprocess) so
that we can still try to import the real module name and fall back to
our compatibility module if we get an ImportError.  The import_as()
function defined below loads the module as the "real" name (without the
'_scons'), after which all of the "import {module}" statements in the
rest of our code will find our pre-loaded compatibility module.
"""
#  """
# SCons 兼容性包用于支持旧版本 Python

# 这个子包包含了为各种功能提供向后兼容实现的模块，这些功能我们希望在 SCons 中使用，
# 但只在比我们仍支持的早期旧版本 Python 更新的版本中才可用。

# 其他代码通常不会通过 SCons.compat 命名空间引用此包中的内容。这里的模块会将内容添加到
# __builtin__ 命名空间或全局模块列表中，这样我们的其余代码就可以无论 Python 版本如何
# 都能使用这里导入的对象和名称。

# 简单来说，需要放入 __builtin__ 命名空间的内容来自我们的 builtins 模块。

# 这里其余的内容将位于单独的兼容性模块中，这些模块要么是：1) 我们想要使用的未来模块的
# 适当修改副本；要么是 2) 我们想要使用的未来模块 API 特定部分的向后兼容重新实现。

# 一般警告：SCons.compat 模块中的函数实现不能保证与后续 Python 版本中的这些函数完全兼容。
# 我们只关心添加在 SCons 中实际使用的功能，所以如果你将此代码用于其他用途时要小心。
# （话虽如此，让这些实现更接近后续的官方版本仍然是一个理想目标，我们只是不需要过分执着于此。）

# 我们将兼容性模块命名为以 '_scons_' 开头（例如，_scons_subprocess.py 是我们为 subprocess
# 准备的兼容性模块），这样我们仍然可以尝试导入真实模块名，如果得到 ImportError 时可以回退到
# 我们的兼容性模块。下面定义的 import_as() 函数以"真实"名称（不带 '_scons'）加载模块，
# 之后我们代码中所有 "import {module}" 语句都会找到我们预加载的兼容性模块。
# """
__revision__ = "src/engine/SCons/compat/__init__.py 3842 2008/12/20 22:59:52 scons"

def import_as(module, name):
    """
    Imports the specified module (from our local directory) as the
    specified name.
    """
    import imp
    import os.path
    dir = os.path.split(__file__)[0]
    file, filename, suffix_mode_type = imp.find_module(module, [dir])
    imp.load_module(name, file, filename, suffix_mode_type)

import builtins

try:
    import hashlib
except ImportError:
    # Pre-2.5 Python has no hashlib module.
    try:
        import_as('_scons_hashlib', 'hashlib')
    except ImportError:
        # If we failed importing our compatibility module, it probably
        # means this version of Python has no md5 module.  Don't do
        # anything and let the higher layer discover this fact, so it
        # can fall back to using timestamp.
        pass

try:
    set
except NameError:
    # Pre-2.4 Python has no native set type
    try:
        # Python 2.2 and 2.3 can use the copy of the 2.[45] sets module
        # that we grabbed.
        import_as('_scons_sets', 'sets')
    except (ImportError, SyntaxError):
        # Python 1.5 (ImportError, no __future_ module) and 2.1
        # (SyntaxError, no generators in __future__) will blow up
        # trying to import the 2.[45] sets module, so back off to a
        # custom sets module that can be discarded easily when we
        # stop supporting those versions.
        import_as('_scons_sets15', 'sets')
    import __builtin__
    import sets
    __builtin__.set = sets.Set

import fnmatch
try:
    fnmatch.filter
except AttributeError:
    # Pre-2.2 Python has no fnmatch.filter() function.
    def filter(names, pat):
        """Return the subset of the list NAMES that match PAT"""
        import os,posixpath
        result=[]
        pat = os.path.normcase(pat)
        if not fnmatch._cache.has_key(pat):
            import re
            res = fnmatch.translate(pat)
            fnmatch._cache[pat] = re.compile(res)
        match = fnmatch._cache[pat].match
        if os.path is posixpath:
            # normcase on posix is NOP. Optimize it away from the loop.
            for name in names:
                if match(name):
                    result.append(name)
        else:
            for name in names:
                if match(os.path.normcase(name)):
                    result.append(name)
        return result
    fnmatch.filter = filter
    del filter

try:
    import itertools
except ImportError:
    # Pre-2.3 Python has no itertools module.
    import_as('_scons_itertools', 'itertools')

# If we need the compatibility version of textwrap, it  must be imported
# before optparse, which uses it.
try:
    import textwrap
except ImportError:
    # Pre-2.3 Python has no textwrap module.
    import_as('_scons_textwrap', 'textwrap')

try:
    import optparse
except ImportError:
    # Pre-2.3 Python has no optparse module.
    import_as('_scons_optparse', 'optparse')

import os
try:
    os.devnull
except AttributeError:
    # Pre-2.4 Python has no os.devnull attribute
    import sys
    _names = sys.builtin_module_names
    if 'posix' in _names:
        os.devnull = '/dev/null'
    elif 'nt' in _names:
        os.devnull = 'nul'
    os.path.devnull = os.devnull

import shlex
try:
    shlex.split
except AttributeError:
    # Pre-2.3 Python has no shlex.split() function.
    #
    # The full white-space splitting semantics of shlex.split() are
    # complicated to reproduce by hand, so just use a compatibility
    # version of the shlex module cribbed from Python 2.5 with some
    # minor modifications for older Python versions.
    del shlex
    import_as('_scons_shlex', 'shlex')


import shutil
try:
    shutil.move
except AttributeError:
    # Pre-2.3 Python has no shutil.move() function.
    #
    # Cribbed from Python 2.5.
    import os

    def move(src, dst):
        """Recursively move a file or directory to another location.

        If the destination is on our current filesystem, then simply use
        rename.  Otherwise, copy src to the dst and then remove src.
        A lot more could be done here...  A look at a mv.c shows a lot of
        the issues this implementation glosses over.

        """
        try:
            os.rename(src, dst)
        except OSError:
            if os.path.isdir(src):
                if shutil.destinsrc(src, dst):
                    raise Error, "Cannot move a directory '%s' into itself '%s'." % (src, dst)
                shutil.copytree(src, dst, symlinks=True)
                shutil.rmtree(src)
            else:
                shutil.copy2(src,dst)
                os.unlink(src)
    shutil.move = move
    del move

    def destinsrc(src, dst):
        src = os.path.abspath(src)
        return os.path.abspath(dst)[:len(src)] == src
    shutil.destinsrc = destinsrc
    del destinsrc


try:
    import subprocess
except ImportError:
    # Pre-2.4 Python has no subprocess module.
    import_as('_scons_subprocess', 'subprocess')

import sys
try:
    sys.version_info
except AttributeError:
    # Pre-1.6 Python has no sys.version_info
    import string
    version_string = string.split(sys.version)[0]
    version_ints = map(int, string.split(version_string, '.'))
    sys.version_info = tuple(version_ints + ['final', 0])

try:
    import UserString
except ImportError:
    # Pre-1.6 Python has no UserString module.
    import_as('_scons_UserString', 'UserString')
