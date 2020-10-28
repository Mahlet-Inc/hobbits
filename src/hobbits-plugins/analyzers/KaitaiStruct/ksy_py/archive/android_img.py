# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
import collections


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class AndroidImg(KaitaiStruct):
    """
    .. seealso::
       Source - https://source.android.com/devices/bootloader/boot-image-header
    """
    SEQ_FIELDS = ["magic", "kernel", "ramdisk", "second", "tags_load", "page_size", "header_version", "os_version", "name", "cmdline", "sha", "extra_cmdline", "recovery_dtbo", "boot_header_size", "dtb"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['magic']['start'] = self._io.pos()
        self.magic = self._io.read_bytes(8)
        self._debug['magic']['end'] = self._io.pos()
        if not self.magic == b"\x41\x4E\x44\x52\x4F\x49\x44\x21":
            raise kaitaistruct.ValidationNotEqualError(b"\x41\x4E\x44\x52\x4F\x49\x44\x21", self.magic, self._io, u"/seq/0")
        self._debug['kernel']['start'] = self._io.pos()
        self.kernel = AndroidImg.Load(self._io, self, self._root)
        self.kernel._read()
        self._debug['kernel']['end'] = self._io.pos()
        self._debug['ramdisk']['start'] = self._io.pos()
        self.ramdisk = AndroidImg.Load(self._io, self, self._root)
        self.ramdisk._read()
        self._debug['ramdisk']['end'] = self._io.pos()
        self._debug['second']['start'] = self._io.pos()
        self.second = AndroidImg.Load(self._io, self, self._root)
        self.second._read()
        self._debug['second']['end'] = self._io.pos()
        self._debug['tags_load']['start'] = self._io.pos()
        self.tags_load = self._io.read_u4le()
        self._debug['tags_load']['end'] = self._io.pos()
        self._debug['page_size']['start'] = self._io.pos()
        self.page_size = self._io.read_u4le()
        self._debug['page_size']['end'] = self._io.pos()
        self._debug['header_version']['start'] = self._io.pos()
        self.header_version = self._io.read_u4le()
        self._debug['header_version']['end'] = self._io.pos()
        self._debug['os_version']['start'] = self._io.pos()
        self.os_version = AndroidImg.OsVersion(self._io, self, self._root)
        self.os_version._read()
        self._debug['os_version']['end'] = self._io.pos()
        self._debug['name']['start'] = self._io.pos()
        self.name = (KaitaiStream.bytes_terminate(self._io.read_bytes(16), 0, False)).decode(u"ASCII")
        self._debug['name']['end'] = self._io.pos()
        self._debug['cmdline']['start'] = self._io.pos()
        self.cmdline = (KaitaiStream.bytes_terminate(self._io.read_bytes(512), 0, False)).decode(u"ASCII")
        self._debug['cmdline']['end'] = self._io.pos()
        self._debug['sha']['start'] = self._io.pos()
        self.sha = self._io.read_bytes(32)
        self._debug['sha']['end'] = self._io.pos()
        self._debug['extra_cmdline']['start'] = self._io.pos()
        self.extra_cmdline = (KaitaiStream.bytes_terminate(self._io.read_bytes(1024), 0, False)).decode(u"ASCII")
        self._debug['extra_cmdline']['end'] = self._io.pos()
        if self.header_version > 0:
            self._debug['recovery_dtbo']['start'] = self._io.pos()
            self.recovery_dtbo = AndroidImg.SizeOffset(self._io, self, self._root)
            self.recovery_dtbo._read()
            self._debug['recovery_dtbo']['end'] = self._io.pos()

        if self.header_version > 0:
            self._debug['boot_header_size']['start'] = self._io.pos()
            self.boot_header_size = self._io.read_u4le()
            self._debug['boot_header_size']['end'] = self._io.pos()

        if self.header_version > 1:
            self._debug['dtb']['start'] = self._io.pos()
            self.dtb = AndroidImg.LoadLong(self._io, self, self._root)
            self.dtb._read()
            self._debug['dtb']['end'] = self._io.pos()


    class Load(KaitaiStruct):
        SEQ_FIELDS = ["size", "addr"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['size']['start'] = self._io.pos()
            self.size = self._io.read_u4le()
            self._debug['size']['end'] = self._io.pos()
            self._debug['addr']['start'] = self._io.pos()
            self.addr = self._io.read_u4le()
            self._debug['addr']['end'] = self._io.pos()


    class LoadLong(KaitaiStruct):
        SEQ_FIELDS = ["size", "addr"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['size']['start'] = self._io.pos()
            self.size = self._io.read_u4le()
            self._debug['size']['end'] = self._io.pos()
            self._debug['addr']['start'] = self._io.pos()
            self.addr = self._io.read_u8le()
            self._debug['addr']['end'] = self._io.pos()


    class SizeOffset(KaitaiStruct):
        SEQ_FIELDS = ["size", "offset"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['size']['start'] = self._io.pos()
            self.size = self._io.read_u4le()
            self._debug['size']['end'] = self._io.pos()
            self._debug['offset']['start'] = self._io.pos()
            self.offset = self._io.read_u8le()
            self._debug['offset']['end'] = self._io.pos()


    class OsVersion(KaitaiStruct):
        SEQ_FIELDS = ["version"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['version']['start'] = self._io.pos()
            self.version = self._io.read_u4le()
            self._debug['version']['end'] = self._io.pos()

        @property
        def month(self):
            if hasattr(self, '_m_month'):
                return self._m_month if hasattr(self, '_m_month') else None

            self._m_month = (self.version & 15)
            return self._m_month if hasattr(self, '_m_month') else None

        @property
        def patch(self):
            if hasattr(self, '_m_patch'):
                return self._m_patch if hasattr(self, '_m_patch') else None

            self._m_patch = ((self.version >> 11) & 127)
            return self._m_patch if hasattr(self, '_m_patch') else None

        @property
        def year(self):
            if hasattr(self, '_m_year'):
                return self._m_year if hasattr(self, '_m_year') else None

            self._m_year = (((self.version >> 4) & 127) + 2000)
            return self._m_year if hasattr(self, '_m_year') else None

        @property
        def major(self):
            if hasattr(self, '_m_major'):
                return self._m_major if hasattr(self, '_m_major') else None

            self._m_major = ((self.version >> 25) & 127)
            return self._m_major if hasattr(self, '_m_major') else None

        @property
        def minor(self):
            if hasattr(self, '_m_minor'):
                return self._m_minor if hasattr(self, '_m_minor') else None

            self._m_minor = ((self.version >> 18) & 127)
            return self._m_minor if hasattr(self, '_m_minor') else None


    @property
    def kernel_img(self):
        if hasattr(self, '_m_kernel_img'):
            return self._m_kernel_img if hasattr(self, '_m_kernel_img') else None

        _pos = self._io.pos()
        self._io.seek(self.page_size)
        self._debug['_m_kernel_img']['start'] = self._io.pos()
        self._m_kernel_img = self._io.read_bytes(self.kernel.size)
        self._debug['_m_kernel_img']['end'] = self._io.pos()
        self._io.seek(_pos)
        return self._m_kernel_img if hasattr(self, '_m_kernel_img') else None

    @property
    def tags_offset(self):
        """tags offset from base."""
        if hasattr(self, '_m_tags_offset'):
            return self._m_tags_offset if hasattr(self, '_m_tags_offset') else None

        self._m_tags_offset = (self.tags_load - self.base)
        return self._m_tags_offset if hasattr(self, '_m_tags_offset') else None

    @property
    def ramdisk_offset(self):
        """ramdisk offset from base."""
        if hasattr(self, '_m_ramdisk_offset'):
            return self._m_ramdisk_offset if hasattr(self, '_m_ramdisk_offset') else None

        self._m_ramdisk_offset = ((self.ramdisk.addr - self.base) if self.ramdisk.addr > 0 else 0)
        return self._m_ramdisk_offset if hasattr(self, '_m_ramdisk_offset') else None

    @property
    def second_offset(self):
        """2nd bootloader offset from base."""
        if hasattr(self, '_m_second_offset'):
            return self._m_second_offset if hasattr(self, '_m_second_offset') else None

        self._m_second_offset = ((self.second.addr - self.base) if self.second.addr > 0 else 0)
        return self._m_second_offset if hasattr(self, '_m_second_offset') else None

    @property
    def kernel_offset(self):
        """kernel offset from base."""
        if hasattr(self, '_m_kernel_offset'):
            return self._m_kernel_offset if hasattr(self, '_m_kernel_offset') else None

        self._m_kernel_offset = (self.kernel.addr - self.base)
        return self._m_kernel_offset if hasattr(self, '_m_kernel_offset') else None

    @property
    def dtb_offset(self):
        """dtb offset from base."""
        if hasattr(self, '_m_dtb_offset'):
            return self._m_dtb_offset if hasattr(self, '_m_dtb_offset') else None

        if self.header_version > 1:
            self._m_dtb_offset = ((self.dtb.addr - self.base) if self.dtb.addr > 0 else 0)

        return self._m_dtb_offset if hasattr(self, '_m_dtb_offset') else None

    @property
    def dtb_img(self):
        if hasattr(self, '_m_dtb_img'):
            return self._m_dtb_img if hasattr(self, '_m_dtb_img') else None

        if  ((self.header_version > 1) and (self.dtb.size > 0)) :
            _pos = self._io.pos()
            self._io.seek((((((((self.page_size + self.kernel.size) + self.ramdisk.size) + self.second.size) + self.recovery_dtbo.size) + self.page_size) - 1) // self.page_size * self.page_size))
            self._debug['_m_dtb_img']['start'] = self._io.pos()
            self._m_dtb_img = self._io.read_bytes(self.dtb.size)
            self._debug['_m_dtb_img']['end'] = self._io.pos()
            self._io.seek(_pos)

        return self._m_dtb_img if hasattr(self, '_m_dtb_img') else None

    @property
    def ramdisk_img(self):
        if hasattr(self, '_m_ramdisk_img'):
            return self._m_ramdisk_img if hasattr(self, '_m_ramdisk_img') else None

        if self.ramdisk.size > 0:
            _pos = self._io.pos()
            self._io.seek(((((self.page_size + self.kernel.size) + self.page_size) - 1) // self.page_size * self.page_size))
            self._debug['_m_ramdisk_img']['start'] = self._io.pos()
            self._m_ramdisk_img = self._io.read_bytes(self.ramdisk.size)
            self._debug['_m_ramdisk_img']['end'] = self._io.pos()
            self._io.seek(_pos)

        return self._m_ramdisk_img if hasattr(self, '_m_ramdisk_img') else None

    @property
    def recovery_dtbo_img(self):
        if hasattr(self, '_m_recovery_dtbo_img'):
            return self._m_recovery_dtbo_img if hasattr(self, '_m_recovery_dtbo_img') else None

        if  ((self.header_version > 0) and (self.recovery_dtbo.size > 0)) :
            _pos = self._io.pos()
            self._io.seek(self.recovery_dtbo.offset)
            self._debug['_m_recovery_dtbo_img']['start'] = self._io.pos()
            self._m_recovery_dtbo_img = self._io.read_bytes(self.recovery_dtbo.size)
            self._debug['_m_recovery_dtbo_img']['end'] = self._io.pos()
            self._io.seek(_pos)

        return self._m_recovery_dtbo_img if hasattr(self, '_m_recovery_dtbo_img') else None

    @property
    def second_img(self):
        if hasattr(self, '_m_second_img'):
            return self._m_second_img if hasattr(self, '_m_second_img') else None

        if self.second.size > 0:
            _pos = self._io.pos()
            self._io.seek((((((self.page_size + self.kernel.size) + self.ramdisk.size) + self.page_size) - 1) // self.page_size * self.page_size))
            self._debug['_m_second_img']['start'] = self._io.pos()
            self._m_second_img = self._io.read_bytes(self.second.size)
            self._debug['_m_second_img']['end'] = self._io.pos()
            self._io.seek(_pos)

        return self._m_second_img if hasattr(self, '_m_second_img') else None

    @property
    def base(self):
        """base loading address."""
        if hasattr(self, '_m_base'):
            return self._m_base if hasattr(self, '_m_base') else None

        self._m_base = (self.kernel.addr - 32768)
        return self._m_base if hasattr(self, '_m_base') else None


