# Copyright (C) DevCartel Co.,Ltd.
# Email: support@devcartel.com
import sys
__version__ = '8.5.3'

if sys.version_info >= (3, 0):
    from pyrfa.pyrfa import *
else:
    from pyrfa import *
