# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
from enum import Enum
import collections


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class BtrfsStream(KaitaiStruct):
    """Btrfs is a copy on write file system based on B-trees focusing on fault tolerance, repair and easy
    administration. Btrfs is intended to address the lack of pooling, snapshots, checksums, and
    integral multi-device spanning in Linux file systems.
    Given any pair of subvolumes (or snapshots), Btrfs can generate a binary diff between them by
    using the `btrfs send` command that can be replayed later by using `btrfs receive`, possibly on a
    different Btrfs file system. The `btrfs send` command creates a set of data modifications required
    for converting one subvolume into another.
    This spec can be used to disassemble the binary diff created by the `btrfs send` command.
    If you want a text representation you may want to checkout `btrfs receive --dump` instead.
    
    .. seealso::
       Source - https://btrfs.wiki.kernel.org/index.php/Design_notes_on_Send/Receive
    """

    class Command(Enum):
        unspec = 0
        subvol = 1
        snapshot = 2
        mkfile = 3
        mkdir = 4
        mknod = 5
        mkfifo = 6
        mksock = 7
        symlink = 8
        rename = 9
        link = 10
        unlink = 11
        rmdir = 12
        set_xattr = 13
        remove_xattr = 14
        write = 15
        clone = 16
        truncate = 17
        chmod = 18
        chown = 19
        utimes = 20
        end = 21
        update_extent = 22

    class Attribute(Enum):
        unspec = 0
        uuid = 1
        ctransid = 2
        ino = 3
        size = 4
        mode = 5
        uid = 6
        gid = 7
        rdev = 8
        ctime = 9
        mtime = 10
        atime = 11
        otime = 12
        xattr_name = 13
        xattr_data = 14
        path = 15
        path_to = 16
        path_link = 17
        file_offset = 18
        data = 19
        clone_uuid = 20
        clone_ctransid = 21
        clone_path = 22
        clone_offset = 23
        clone_len = 24
    SEQ_FIELDS = ["header", "commands"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['header']['start'] = self._io.pos()
        self.header = BtrfsStream.SendStreamHeader(self._io, self, self._root)
        self.header._read()
        self._debug['header']['end'] = self._io.pos()
        self._debug['commands']['start'] = self._io.pos()
        self.commands = []
        i = 0
        while not self._io.is_eof():
            if not 'arr' in self._debug['commands']:
                self._debug['commands']['arr'] = []
            self._debug['commands']['arr'].append({'start': self._io.pos()})
            _t_commands = BtrfsStream.SendCommand(self._io, self, self._root)
            _t_commands._read()
            self.commands.append(_t_commands)
            self._debug['commands']['arr'][len(self.commands) - 1]['end'] = self._io.pos()
            i += 1

        self._debug['commands']['end'] = self._io.pos()

    class SendStreamHeader(KaitaiStruct):
        SEQ_FIELDS = ["magic", "version"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['magic']['start'] = self._io.pos()
            self.magic = self._io.read_bytes(13)
            self._debug['magic']['end'] = self._io.pos()
            if not self.magic == b"\x62\x74\x72\x66\x73\x2D\x73\x74\x72\x65\x61\x6D\x00":
                raise kaitaistruct.ValidationNotEqualError(b"\x62\x74\x72\x66\x73\x2D\x73\x74\x72\x65\x61\x6D\x00", self.magic, self._io, u"/types/send_stream_header/seq/0")
            self._debug['version']['start'] = self._io.pos()
            self.version = self._io.read_u4le()
            self._debug['version']['end'] = self._io.pos()


    class SendCommand(KaitaiStruct):
        SEQ_FIELDS = ["len_data", "type", "checksum", "data"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['len_data']['start'] = self._io.pos()
            self.len_data = self._io.read_u4le()
            self._debug['len_data']['end'] = self._io.pos()
            self._debug['type']['start'] = self._io.pos()
            self.type = KaitaiStream.resolve_enum(BtrfsStream.Command, self._io.read_u2le())
            self._debug['type']['end'] = self._io.pos()
            self._debug['checksum']['start'] = self._io.pos()
            self.checksum = self._io.read_bytes(4)
            self._debug['checksum']['end'] = self._io.pos()
            self._debug['data']['start'] = self._io.pos()
            self._raw_data = self._io.read_bytes(self.len_data)
            _io__raw_data = KaitaiStream(BytesIO(self._raw_data))
            self.data = BtrfsStream.SendCommand.Tlvs(_io__raw_data, self, self._root)
            self.data._read()
            self._debug['data']['end'] = self._io.pos()

        class Tlv(KaitaiStruct):
            SEQ_FIELDS = ["type", "length", "value"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['type']['start'] = self._io.pos()
                self.type = KaitaiStream.resolve_enum(BtrfsStream.Attribute, self._io.read_u2le())
                self._debug['type']['end'] = self._io.pos()
                self._debug['length']['start'] = self._io.pos()
                self.length = self._io.read_u2le()
                self._debug['length']['end'] = self._io.pos()
                self._debug['value']['start'] = self._io.pos()
                _on = self.type
                if _on == BtrfsStream.Attribute.ctransid:
                    self.value = self._io.read_u8le()
                elif _on == BtrfsStream.Attribute.size:
                    self.value = self._io.read_u8le()
                elif _on == BtrfsStream.Attribute.clone_uuid:
                    self._raw_value = self._io.read_bytes(self.length)
                    _io__raw_value = KaitaiStream(BytesIO(self._raw_value))
                    self.value = BtrfsStream.SendCommand.Uuid(_io__raw_value, self, self._root)
                    self.value._read()
                elif _on == BtrfsStream.Attribute.file_offset:
                    self.value = self._io.read_u8le()
                elif _on == BtrfsStream.Attribute.otime:
                    self._raw_value = self._io.read_bytes(self.length)
                    _io__raw_value = KaitaiStream(BytesIO(self._raw_value))
                    self.value = BtrfsStream.SendCommand.Timespec(_io__raw_value, self, self._root)
                    self.value._read()
                elif _on == BtrfsStream.Attribute.uid:
                    self.value = self._io.read_u8le()
                elif _on == BtrfsStream.Attribute.atime:
                    self._raw_value = self._io.read_bytes(self.length)
                    _io__raw_value = KaitaiStream(BytesIO(self._raw_value))
                    self.value = BtrfsStream.SendCommand.Timespec(_io__raw_value, self, self._root)
                    self.value._read()
                elif _on == BtrfsStream.Attribute.ctime:
                    self._raw_value = self._io.read_bytes(self.length)
                    _io__raw_value = KaitaiStream(BytesIO(self._raw_value))
                    self.value = BtrfsStream.SendCommand.Timespec(_io__raw_value, self, self._root)
                    self.value._read()
                elif _on == BtrfsStream.Attribute.uuid:
                    self._raw_value = self._io.read_bytes(self.length)
                    _io__raw_value = KaitaiStream(BytesIO(self._raw_value))
                    self.value = BtrfsStream.SendCommand.Uuid(_io__raw_value, self, self._root)
                    self.value._read()
                elif _on == BtrfsStream.Attribute.clone_len:
                    self.value = self._io.read_u8le()
                elif _on == BtrfsStream.Attribute.xattr_name:
                    self._raw_value = self._io.read_bytes(self.length)
                    _io__raw_value = KaitaiStream(BytesIO(self._raw_value))
                    self.value = BtrfsStream.SendCommand.String(_io__raw_value, self, self._root)
                    self.value._read()
                elif _on == BtrfsStream.Attribute.clone_ctransid:
                    self.value = self._io.read_u8le()
                elif _on == BtrfsStream.Attribute.mode:
                    self.value = self._io.read_u8le()
                elif _on == BtrfsStream.Attribute.mtime:
                    self._raw_value = self._io.read_bytes(self.length)
                    _io__raw_value = KaitaiStream(BytesIO(self._raw_value))
                    self.value = BtrfsStream.SendCommand.Timespec(_io__raw_value, self, self._root)
                    self.value._read()
                elif _on == BtrfsStream.Attribute.path_link:
                    self._raw_value = self._io.read_bytes(self.length)
                    _io__raw_value = KaitaiStream(BytesIO(self._raw_value))
                    self.value = BtrfsStream.SendCommand.String(_io__raw_value, self, self._root)
                    self.value._read()
                elif _on == BtrfsStream.Attribute.rdev:
                    self.value = self._io.read_u8le()
                elif _on == BtrfsStream.Attribute.path_to:
                    self._raw_value = self._io.read_bytes(self.length)
                    _io__raw_value = KaitaiStream(BytesIO(self._raw_value))
                    self.value = BtrfsStream.SendCommand.String(_io__raw_value, self, self._root)
                    self.value._read()
                elif _on == BtrfsStream.Attribute.path:
                    self._raw_value = self._io.read_bytes(self.length)
                    _io__raw_value = KaitaiStream(BytesIO(self._raw_value))
                    self.value = BtrfsStream.SendCommand.String(_io__raw_value, self, self._root)
                    self.value._read()
                elif _on == BtrfsStream.Attribute.clone_offset:
                    self.value = self._io.read_u8le()
                elif _on == BtrfsStream.Attribute.gid:
                    self.value = self._io.read_u8le()
                elif _on == BtrfsStream.Attribute.clone_path:
                    self._raw_value = self._io.read_bytes(self.length)
                    _io__raw_value = KaitaiStream(BytesIO(self._raw_value))
                    self.value = BtrfsStream.SendCommand.String(_io__raw_value, self, self._root)
                    self.value._read()
                else:
                    self.value = self._io.read_bytes(self.length)
                self._debug['value']['end'] = self._io.pos()


        class Uuid(KaitaiStruct):
            SEQ_FIELDS = ["uuid"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['uuid']['start'] = self._io.pos()
                self.uuid = self._io.read_bytes(16)
                self._debug['uuid']['end'] = self._io.pos()


        class Tlvs(KaitaiStruct):
            SEQ_FIELDS = ["tlv"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['tlv']['start'] = self._io.pos()
                self.tlv = []
                i = 0
                while not self._io.is_eof():
                    if not 'arr' in self._debug['tlv']:
                        self._debug['tlv']['arr'] = []
                    self._debug['tlv']['arr'].append({'start': self._io.pos()})
                    _t_tlv = BtrfsStream.SendCommand.Tlv(self._io, self, self._root)
                    _t_tlv._read()
                    self.tlv.append(_t_tlv)
                    self._debug['tlv']['arr'][len(self.tlv) - 1]['end'] = self._io.pos()
                    i += 1

                self._debug['tlv']['end'] = self._io.pos()


        class String(KaitaiStruct):
            SEQ_FIELDS = ["string"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['string']['start'] = self._io.pos()
                self.string = (self._io.read_bytes_full()).decode(u"UTF-8")
                self._debug['string']['end'] = self._io.pos()


        class Timespec(KaitaiStruct):
            SEQ_FIELDS = ["ts_sec", "ts_nsec"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['ts_sec']['start'] = self._io.pos()
                self.ts_sec = self._io.read_s8le()
                self._debug['ts_sec']['end'] = self._io.pos()
                self._debug['ts_nsec']['start'] = self._io.pos()
                self.ts_nsec = self._io.read_s4le()
                self._debug['ts_nsec']['end'] = self._io.pos()




