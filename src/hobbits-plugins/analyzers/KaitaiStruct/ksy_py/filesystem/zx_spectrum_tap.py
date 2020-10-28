# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
from enum import Enum
import collections


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class ZxSpectrumTap(KaitaiStruct):
    """TAP files are used by emulators of ZX Spectrum computer (released in
    1982 by Sinclair Research). TAP file stores blocks of data as if
    they are written to magnetic tape, which was used as primary media
    for ZX Spectrum. Contents of this file can be viewed as a very
    simple linear filesystem, storing named files with some basic
    metainformation prepended as a header.
    
    .. seealso::
       Source - https://faqwiki.zxnet.co.uk/wiki/TAP_format
    """

    class FlagEnum(Enum):
        header = 0
        data = 255

    class HeaderTypeEnum(Enum):
        program = 0
        num_array = 1
        char_array = 2
        bytes = 3
    SEQ_FIELDS = ["blocks"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['blocks']['start'] = self._io.pos()
        self.blocks = []
        i = 0
        while not self._io.is_eof():
            if not 'arr' in self._debug['blocks']:
                self._debug['blocks']['arr'] = []
            self._debug['blocks']['arr'].append({'start': self._io.pos()})
            _t_blocks = ZxSpectrumTap.Block(self._io, self, self._root)
            _t_blocks._read()
            self.blocks.append(_t_blocks)
            self._debug['blocks']['arr'][len(self.blocks) - 1]['end'] = self._io.pos()
            i += 1

        self._debug['blocks']['end'] = self._io.pos()

    class Block(KaitaiStruct):
        SEQ_FIELDS = ["len_block", "flag", "header", "data", "headerless_data"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['len_block']['start'] = self._io.pos()
            self.len_block = self._io.read_u2le()
            self._debug['len_block']['end'] = self._io.pos()
            self._debug['flag']['start'] = self._io.pos()
            self.flag = KaitaiStream.resolve_enum(ZxSpectrumTap.FlagEnum, self._io.read_u1())
            self._debug['flag']['end'] = self._io.pos()
            if  ((self.len_block == 19) and (self.flag == ZxSpectrumTap.FlagEnum.header)) :
                self._debug['header']['start'] = self._io.pos()
                self.header = ZxSpectrumTap.Header(self._io, self, self._root)
                self.header._read()
                self._debug['header']['end'] = self._io.pos()

            if self.len_block == 19:
                self._debug['data']['start'] = self._io.pos()
                self.data = self._io.read_bytes((self.header.len_data + 4))
                self._debug['data']['end'] = self._io.pos()

            if self.flag == ZxSpectrumTap.FlagEnum.data:
                self._debug['headerless_data']['start'] = self._io.pos()
                self.headerless_data = self._io.read_bytes((self.len_block - 1))
                self._debug['headerless_data']['end'] = self._io.pos()



    class ProgramParams(KaitaiStruct):
        SEQ_FIELDS = ["autostart_line", "len_program"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['autostart_line']['start'] = self._io.pos()
            self.autostart_line = self._io.read_u2le()
            self._debug['autostart_line']['end'] = self._io.pos()
            self._debug['len_program']['start'] = self._io.pos()
            self.len_program = self._io.read_u2le()
            self._debug['len_program']['end'] = self._io.pos()


    class BytesParams(KaitaiStruct):
        SEQ_FIELDS = ["start_address", "reserved"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['start_address']['start'] = self._io.pos()
            self.start_address = self._io.read_u2le()
            self._debug['start_address']['end'] = self._io.pos()
            self._debug['reserved']['start'] = self._io.pos()
            self.reserved = self._io.read_bytes(2)
            self._debug['reserved']['end'] = self._io.pos()


    class Header(KaitaiStruct):
        SEQ_FIELDS = ["header_type", "filename", "len_data", "params", "checksum"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['header_type']['start'] = self._io.pos()
            self.header_type = KaitaiStream.resolve_enum(ZxSpectrumTap.HeaderTypeEnum, self._io.read_u1())
            self._debug['header_type']['end'] = self._io.pos()
            self._debug['filename']['start'] = self._io.pos()
            self.filename = KaitaiStream.bytes_strip_right(self._io.read_bytes(10), 32)
            self._debug['filename']['end'] = self._io.pos()
            self._debug['len_data']['start'] = self._io.pos()
            self.len_data = self._io.read_u2le()
            self._debug['len_data']['end'] = self._io.pos()
            self._debug['params']['start'] = self._io.pos()
            _on = self.header_type
            if _on == ZxSpectrumTap.HeaderTypeEnum.program:
                self.params = ZxSpectrumTap.ProgramParams(self._io, self, self._root)
                self.params._read()
            elif _on == ZxSpectrumTap.HeaderTypeEnum.num_array:
                self.params = ZxSpectrumTap.ArrayParams(self._io, self, self._root)
                self.params._read()
            elif _on == ZxSpectrumTap.HeaderTypeEnum.char_array:
                self.params = ZxSpectrumTap.ArrayParams(self._io, self, self._root)
                self.params._read()
            elif _on == ZxSpectrumTap.HeaderTypeEnum.bytes:
                self.params = ZxSpectrumTap.BytesParams(self._io, self, self._root)
                self.params._read()
            self._debug['params']['end'] = self._io.pos()
            self._debug['checksum']['start'] = self._io.pos()
            self.checksum = self._io.read_u1()
            self._debug['checksum']['end'] = self._io.pos()


    class ArrayParams(KaitaiStruct):
        SEQ_FIELDS = ["reserved", "var_name", "reserved1"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['reserved']['start'] = self._io.pos()
            self.reserved = self._io.read_u1()
            self._debug['reserved']['end'] = self._io.pos()
            self._debug['var_name']['start'] = self._io.pos()
            self.var_name = self._io.read_u1()
            self._debug['var_name']['end'] = self._io.pos()
            self._debug['reserved1']['start'] = self._io.pos()
            self.reserved1 = self._io.read_bytes(2)
            self._debug['reserved1']['end'] = self._io.pos()
            if not self.reserved1 == b"\x00\x80":
                raise kaitaistruct.ValidationNotEqualError(b"\x00\x80", self.reserved1, self._io, u"/types/array_params/seq/2")



