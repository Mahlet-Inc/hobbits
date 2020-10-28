# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
import collections
from enum import Enum


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class SomeIpSdEntries(KaitaiStruct):
    """The entries are used to synchronize the state of services instances and the 
    Publish/-Subscribe handling.
    
    .. seealso::
       - section 4.1.2.3  Entry Format - https://www.autosar.org/fileadmin/user_upload/standards/foundation/19-11/AUTOSAR_PRS_SOMEIPServiceDiscoveryProtocol.pdf
    """
    SEQ_FIELDS = ["entries"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['entries']['start'] = self._io.pos()
        self.entries = []
        i = 0
        while not self._io.is_eof():
            if not 'arr' in self._debug['entries']:
                self._debug['entries']['arr'] = []
            self._debug['entries']['arr'].append({'start': self._io.pos()})
            _t_entries = SomeIpSdEntries.SdEntry(self._io, self, self._root)
            _t_entries._read()
            self.entries.append(_t_entries)
            self._debug['entries']['arr'][len(self.entries) - 1]['end'] = self._io.pos()
            i += 1

        self._debug['entries']['end'] = self._io.pos()

    class SdEntry(KaitaiStruct):

        class EntryTypes(Enum):
            find = 0
            offer = 1
            subscribe = 6
            subscribe_ack = 7
        SEQ_FIELDS = ["header", "content"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['header']['start'] = self._io.pos()
            self.header = SomeIpSdEntries.SdEntry.SdEntryHeader(self._io, self, self._root)
            self.header._read()
            self._debug['header']['end'] = self._io.pos()
            self._debug['content']['start'] = self._io.pos()
            _on = self.header.type
            if _on == SomeIpSdEntries.SdEntry.EntryTypes.find:
                self.content = SomeIpSdEntries.SdEntry.SdServiceEntry(self._io, self, self._root)
                self.content._read()
            elif _on == SomeIpSdEntries.SdEntry.EntryTypes.offer:
                self.content = SomeIpSdEntries.SdEntry.SdServiceEntry(self._io, self, self._root)
                self.content._read()
            elif _on == SomeIpSdEntries.SdEntry.EntryTypes.subscribe:
                self.content = SomeIpSdEntries.SdEntry.SdEventgroupEntry(self._io, self, self._root)
                self.content._read()
            elif _on == SomeIpSdEntries.SdEntry.EntryTypes.subscribe_ack:
                self.content = SomeIpSdEntries.SdEntry.SdEventgroupEntry(self._io, self, self._root)
                self.content._read()
            self._debug['content']['end'] = self._io.pos()

        class SdEntryHeader(KaitaiStruct):
            SEQ_FIELDS = ["type", "index_first_options", "index_second_options", "number_first_options", "number_second_options", "service_id", "instance_id", "major_version", "ttl"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['type']['start'] = self._io.pos()
                self.type = KaitaiStream.resolve_enum(SomeIpSdEntries.SdEntry.EntryTypes, self._io.read_u1())
                self._debug['type']['end'] = self._io.pos()
                self._debug['index_first_options']['start'] = self._io.pos()
                self.index_first_options = self._io.read_u1()
                self._debug['index_first_options']['end'] = self._io.pos()
                self._debug['index_second_options']['start'] = self._io.pos()
                self.index_second_options = self._io.read_u1()
                self._debug['index_second_options']['end'] = self._io.pos()
                self._debug['number_first_options']['start'] = self._io.pos()
                self.number_first_options = self._io.read_bits_int_be(4)
                self._debug['number_first_options']['end'] = self._io.pos()
                self._debug['number_second_options']['start'] = self._io.pos()
                self.number_second_options = self._io.read_bits_int_be(4)
                self._debug['number_second_options']['end'] = self._io.pos()
                self._io.align_to_byte()
                self._debug['service_id']['start'] = self._io.pos()
                self.service_id = self._io.read_u2be()
                self._debug['service_id']['end'] = self._io.pos()
                self._debug['instance_id']['start'] = self._io.pos()
                self.instance_id = self._io.read_u2be()
                self._debug['instance_id']['end'] = self._io.pos()
                self._debug['major_version']['start'] = self._io.pos()
                self.major_version = self._io.read_u1()
                self._debug['major_version']['end'] = self._io.pos()
                self._debug['ttl']['start'] = self._io.pos()
                self.ttl = self._io.read_bits_int_be(24)
                self._debug['ttl']['end'] = self._io.pos()


        class SdServiceEntry(KaitaiStruct):
            SEQ_FIELDS = ["minor_version"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['minor_version']['start'] = self._io.pos()
                self.minor_version = self._io.read_u4be()
                self._debug['minor_version']['end'] = self._io.pos()


        class SdEventgroupEntry(KaitaiStruct):
            SEQ_FIELDS = ["reserved", "initial_data_requested", "reserved2", "counter", "event_group_id"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['reserved']['start'] = self._io.pos()
                self.reserved = self._io.read_u1()
                self._debug['reserved']['end'] = self._io.pos()
                self._debug['initial_data_requested']['start'] = self._io.pos()
                self.initial_data_requested = self._io.read_bits_int_be(1) != 0
                self._debug['initial_data_requested']['end'] = self._io.pos()
                self._debug['reserved2']['start'] = self._io.pos()
                self.reserved2 = self._io.read_bits_int_be(3)
                self._debug['reserved2']['end'] = self._io.pos()
                self._debug['counter']['start'] = self._io.pos()
                self.counter = self._io.read_bits_int_be(4)
                self._debug['counter']['end'] = self._io.pos()
                self._io.align_to_byte()
                self._debug['event_group_id']['start'] = self._io.pos()
                self.event_group_id = self._io.read_u2be()
                self._debug['event_group_id']['end'] = self._io.pos()




