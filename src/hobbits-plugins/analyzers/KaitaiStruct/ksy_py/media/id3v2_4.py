# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
import collections


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class Id3v24(KaitaiStruct):
    SEQ_FIELDS = ["tag"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['tag']['start'] = self._io.pos()
        self.tag = Id3v24.Tag(self._io, self, self._root)
        self.tag._read()
        self._debug['tag']['end'] = self._io.pos()

    class U1beSynchsafe(KaitaiStruct):
        SEQ_FIELDS = ["padding", "value"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['padding']['start'] = self._io.pos()
            self.padding = self._io.read_bits_int_be(1) != 0
            self._debug['padding']['end'] = self._io.pos()
            self._debug['value']['start'] = self._io.pos()
            self.value = self._io.read_bits_int_be(7)
            self._debug['value']['end'] = self._io.pos()


    class U2beSynchsafe(KaitaiStruct):
        SEQ_FIELDS = ["byte0", "byte1"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['byte0']['start'] = self._io.pos()
            self.byte0 = Id3v24.U1beSynchsafe(self._io, self, self._root)
            self.byte0._read()
            self._debug['byte0']['end'] = self._io.pos()
            self._debug['byte1']['start'] = self._io.pos()
            self.byte1 = Id3v24.U1beSynchsafe(self._io, self, self._root)
            self.byte1._read()
            self._debug['byte1']['end'] = self._io.pos()

        @property
        def value(self):
            if hasattr(self, '_m_value'):
                return self._m_value if hasattr(self, '_m_value') else None

            self._m_value = ((self.byte0.value << 7) | self.byte1.value)
            return self._m_value if hasattr(self, '_m_value') else None


    class Tag(KaitaiStruct):
        SEQ_FIELDS = ["header", "header_ex", "frames", "padding", "footer"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['header']['start'] = self._io.pos()
            self.header = Id3v24.Header(self._io, self, self._root)
            self.header._read()
            self._debug['header']['end'] = self._io.pos()
            if self.header.flags.flag_headerex:
                self._debug['header_ex']['start'] = self._io.pos()
                self.header_ex = Id3v24.HeaderEx(self._io, self, self._root)
                self.header_ex._read()
                self._debug['header_ex']['end'] = self._io.pos()

            self._debug['frames']['start'] = self._io.pos()
            self.frames = []
            i = 0
            while True:
                if not 'arr' in self._debug['frames']:
                    self._debug['frames']['arr'] = []
                self._debug['frames']['arr'].append({'start': self._io.pos()})
                _t_frames = Id3v24.Frame(self._io, self, self._root)
                _t_frames._read()
                _ = _t_frames
                self.frames.append(_)
                self._debug['frames']['arr'][len(self.frames) - 1]['end'] = self._io.pos()
                if  (((self._io.pos() + _.size.value) > self.header.size.value) or (_.is_invalid)) :
                    break
                i += 1
            self._debug['frames']['end'] = self._io.pos()
            if not (self.header.flags.flag_footer):
                self._debug['padding']['start'] = self._io.pos()
                self.padding = Id3v24.Padding(self._io, self, self._root)
                self.padding._read()
                self._debug['padding']['end'] = self._io.pos()

            if self.header.flags.flag_footer:
                self._debug['footer']['start'] = self._io.pos()
                self.footer = Id3v24.Footer(self._io, self, self._root)
                self.footer._read()
                self._debug['footer']['end'] = self._io.pos()



    class U4beSynchsafe(KaitaiStruct):
        SEQ_FIELDS = ["short0", "short1"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['short0']['start'] = self._io.pos()
            self.short0 = Id3v24.U2beSynchsafe(self._io, self, self._root)
            self.short0._read()
            self._debug['short0']['end'] = self._io.pos()
            self._debug['short1']['start'] = self._io.pos()
            self.short1 = Id3v24.U2beSynchsafe(self._io, self, self._root)
            self.short1._read()
            self._debug['short1']['end'] = self._io.pos()

        @property
        def value(self):
            if hasattr(self, '_m_value'):
                return self._m_value if hasattr(self, '_m_value') else None

            self._m_value = ((self.short0.value << 14) | self.short1.value)
            return self._m_value if hasattr(self, '_m_value') else None


    class Frame(KaitaiStruct):
        SEQ_FIELDS = ["id", "size", "flags_status", "flags_format", "data"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['id']['start'] = self._io.pos()
            self.id = (self._io.read_bytes(4)).decode(u"ASCII")
            self._debug['id']['end'] = self._io.pos()
            self._debug['size']['start'] = self._io.pos()
            self.size = Id3v24.U4beSynchsafe(self._io, self, self._root)
            self.size._read()
            self._debug['size']['end'] = self._io.pos()
            self._debug['flags_status']['start'] = self._io.pos()
            self.flags_status = Id3v24.Frame.FlagsStatus(self._io, self, self._root)
            self.flags_status._read()
            self._debug['flags_status']['end'] = self._io.pos()
            self._debug['flags_format']['start'] = self._io.pos()
            self.flags_format = Id3v24.Frame.FlagsFormat(self._io, self, self._root)
            self.flags_format._read()
            self._debug['flags_format']['end'] = self._io.pos()
            self._debug['data']['start'] = self._io.pos()
            self.data = self._io.read_bytes(self.size.value)
            self._debug['data']['end'] = self._io.pos()

        class FlagsStatus(KaitaiStruct):
            SEQ_FIELDS = ["reserved1", "flag_discard_alter_tag", "flag_discard_alter_file", "flag_read_only", "reserved2"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['reserved1']['start'] = self._io.pos()
                self.reserved1 = self._io.read_bits_int_be(1) != 0
                self._debug['reserved1']['end'] = self._io.pos()
                self._debug['flag_discard_alter_tag']['start'] = self._io.pos()
                self.flag_discard_alter_tag = self._io.read_bits_int_be(1) != 0
                self._debug['flag_discard_alter_tag']['end'] = self._io.pos()
                self._debug['flag_discard_alter_file']['start'] = self._io.pos()
                self.flag_discard_alter_file = self._io.read_bits_int_be(1) != 0
                self._debug['flag_discard_alter_file']['end'] = self._io.pos()
                self._debug['flag_read_only']['start'] = self._io.pos()
                self.flag_read_only = self._io.read_bits_int_be(1) != 0
                self._debug['flag_read_only']['end'] = self._io.pos()
                self._debug['reserved2']['start'] = self._io.pos()
                self.reserved2 = self._io.read_bits_int_be(4)
                self._debug['reserved2']['end'] = self._io.pos()


        class FlagsFormat(KaitaiStruct):
            SEQ_FIELDS = ["reserved1", "flag_grouping", "reserved2", "flag_compressed", "flag_encrypted", "flag_unsynchronisated", "flag_indicator"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['reserved1']['start'] = self._io.pos()
                self.reserved1 = self._io.read_bits_int_be(1) != 0
                self._debug['reserved1']['end'] = self._io.pos()
                self._debug['flag_grouping']['start'] = self._io.pos()
                self.flag_grouping = self._io.read_bits_int_be(1) != 0
                self._debug['flag_grouping']['end'] = self._io.pos()
                self._debug['reserved2']['start'] = self._io.pos()
                self.reserved2 = self._io.read_bits_int_be(2)
                self._debug['reserved2']['end'] = self._io.pos()
                self._debug['flag_compressed']['start'] = self._io.pos()
                self.flag_compressed = self._io.read_bits_int_be(1) != 0
                self._debug['flag_compressed']['end'] = self._io.pos()
                self._debug['flag_encrypted']['start'] = self._io.pos()
                self.flag_encrypted = self._io.read_bits_int_be(1) != 0
                self._debug['flag_encrypted']['end'] = self._io.pos()
                self._debug['flag_unsynchronisated']['start'] = self._io.pos()
                self.flag_unsynchronisated = self._io.read_bits_int_be(1) != 0
                self._debug['flag_unsynchronisated']['end'] = self._io.pos()
                self._debug['flag_indicator']['start'] = self._io.pos()
                self.flag_indicator = self._io.read_bits_int_be(1) != 0
                self._debug['flag_indicator']['end'] = self._io.pos()


        @property
        def is_invalid(self):
            if hasattr(self, '_m_is_invalid'):
                return self._m_is_invalid if hasattr(self, '_m_is_invalid') else None

            self._m_is_invalid = self.id == u"\000\000\000\000"
            return self._m_is_invalid if hasattr(self, '_m_is_invalid') else None


    class HeaderEx(KaitaiStruct):
        SEQ_FIELDS = ["size", "flags_ex", "data"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['size']['start'] = self._io.pos()
            self.size = Id3v24.U4beSynchsafe(self._io, self, self._root)
            self.size._read()
            self._debug['size']['end'] = self._io.pos()
            self._debug['flags_ex']['start'] = self._io.pos()
            self.flags_ex = Id3v24.HeaderEx.FlagsEx(self._io, self, self._root)
            self.flags_ex._read()
            self._debug['flags_ex']['end'] = self._io.pos()
            self._debug['data']['start'] = self._io.pos()
            self.data = self._io.read_bytes((self.size.value - 5))
            self._debug['data']['end'] = self._io.pos()

        class FlagsEx(KaitaiStruct):
            SEQ_FIELDS = ["reserved1", "flag_update", "flag_crc", "flag_restrictions", "reserved2"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['reserved1']['start'] = self._io.pos()
                self.reserved1 = self._io.read_bits_int_be(1) != 0
                self._debug['reserved1']['end'] = self._io.pos()
                self._debug['flag_update']['start'] = self._io.pos()
                self.flag_update = self._io.read_bits_int_be(1) != 0
                self._debug['flag_update']['end'] = self._io.pos()
                self._debug['flag_crc']['start'] = self._io.pos()
                self.flag_crc = self._io.read_bits_int_be(1) != 0
                self._debug['flag_crc']['end'] = self._io.pos()
                self._debug['flag_restrictions']['start'] = self._io.pos()
                self.flag_restrictions = self._io.read_bits_int_be(1) != 0
                self._debug['flag_restrictions']['end'] = self._io.pos()
                self._debug['reserved2']['start'] = self._io.pos()
                self.reserved2 = self._io.read_bits_int_be(4)
                self._debug['reserved2']['end'] = self._io.pos()



    class Header(KaitaiStruct):
        SEQ_FIELDS = ["magic", "version_major", "version_revision", "flags", "size"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['magic']['start'] = self._io.pos()
            self.magic = self._io.read_bytes(3)
            self._debug['magic']['end'] = self._io.pos()
            if not self.magic == b"\x49\x44\x33":
                raise kaitaistruct.ValidationNotEqualError(b"\x49\x44\x33", self.magic, self._io, u"/types/header/seq/0")
            self._debug['version_major']['start'] = self._io.pos()
            self.version_major = self._io.read_u1()
            self._debug['version_major']['end'] = self._io.pos()
            self._debug['version_revision']['start'] = self._io.pos()
            self.version_revision = self._io.read_u1()
            self._debug['version_revision']['end'] = self._io.pos()
            self._debug['flags']['start'] = self._io.pos()
            self.flags = Id3v24.Header.Flags(self._io, self, self._root)
            self.flags._read()
            self._debug['flags']['end'] = self._io.pos()
            self._debug['size']['start'] = self._io.pos()
            self.size = Id3v24.U4beSynchsafe(self._io, self, self._root)
            self.size._read()
            self._debug['size']['end'] = self._io.pos()

        class Flags(KaitaiStruct):
            SEQ_FIELDS = ["flag_unsynchronization", "flag_headerex", "flag_experimental", "flag_footer", "reserved"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['flag_unsynchronization']['start'] = self._io.pos()
                self.flag_unsynchronization = self._io.read_bits_int_be(1) != 0
                self._debug['flag_unsynchronization']['end'] = self._io.pos()
                self._debug['flag_headerex']['start'] = self._io.pos()
                self.flag_headerex = self._io.read_bits_int_be(1) != 0
                self._debug['flag_headerex']['end'] = self._io.pos()
                self._debug['flag_experimental']['start'] = self._io.pos()
                self.flag_experimental = self._io.read_bits_int_be(1) != 0
                self._debug['flag_experimental']['end'] = self._io.pos()
                self._debug['flag_footer']['start'] = self._io.pos()
                self.flag_footer = self._io.read_bits_int_be(1) != 0
                self._debug['flag_footer']['end'] = self._io.pos()
                self._debug['reserved']['start'] = self._io.pos()
                self.reserved = self._io.read_bits_int_be(4)
                self._debug['reserved']['end'] = self._io.pos()



    class Padding(KaitaiStruct):
        SEQ_FIELDS = ["padding"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['padding']['start'] = self._io.pos()
            self.padding = self._io.read_bytes((self._root.tag.header.size.value - self._io.pos()))
            self._debug['padding']['end'] = self._io.pos()


    class Footer(KaitaiStruct):
        SEQ_FIELDS = ["magic", "version_major", "version_revision", "flags", "size"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['magic']['start'] = self._io.pos()
            self.magic = self._io.read_bytes(3)
            self._debug['magic']['end'] = self._io.pos()
            if not self.magic == b"\x33\x44\x49":
                raise kaitaistruct.ValidationNotEqualError(b"\x33\x44\x49", self.magic, self._io, u"/types/footer/seq/0")
            self._debug['version_major']['start'] = self._io.pos()
            self.version_major = self._io.read_u1()
            self._debug['version_major']['end'] = self._io.pos()
            self._debug['version_revision']['start'] = self._io.pos()
            self.version_revision = self._io.read_u1()
            self._debug['version_revision']['end'] = self._io.pos()
            self._debug['flags']['start'] = self._io.pos()
            self.flags = Id3v24.Footer.Flags(self._io, self, self._root)
            self.flags._read()
            self._debug['flags']['end'] = self._io.pos()
            self._debug['size']['start'] = self._io.pos()
            self.size = Id3v24.U4beSynchsafe(self._io, self, self._root)
            self.size._read()
            self._debug['size']['end'] = self._io.pos()

        class Flags(KaitaiStruct):
            SEQ_FIELDS = ["flag_unsynchronization", "flag_headerex", "flag_experimental", "flag_footer", "reserved"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['flag_unsynchronization']['start'] = self._io.pos()
                self.flag_unsynchronization = self._io.read_bits_int_be(1) != 0
                self._debug['flag_unsynchronization']['end'] = self._io.pos()
                self._debug['flag_headerex']['start'] = self._io.pos()
                self.flag_headerex = self._io.read_bits_int_be(1) != 0
                self._debug['flag_headerex']['end'] = self._io.pos()
                self._debug['flag_experimental']['start'] = self._io.pos()
                self.flag_experimental = self._io.read_bits_int_be(1) != 0
                self._debug['flag_experimental']['end'] = self._io.pos()
                self._debug['flag_footer']['start'] = self._io.pos()
                self.flag_footer = self._io.read_bits_int_be(1) != 0
                self._debug['flag_footer']['end'] = self._io.pos()
                self._debug['reserved']['start'] = self._io.pos()
                self.reserved = self._io.read_bits_int_be(4)
                self._debug['reserved']['end'] = self._io.pos()




