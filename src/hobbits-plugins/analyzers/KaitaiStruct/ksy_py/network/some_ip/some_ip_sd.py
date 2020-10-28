# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
import collections


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

import some_ip_sd_entries
import some_ip_sd_options
class SomeIpSd(KaitaiStruct):
    """The main tasks of the Service Discovery Protocol are communicating the
    availability of functional entities called services in the in-vehicle
    communication as well as controlling the send behavior of event messages.
    This allows sending only event messages to receivers requiring them (Publish/Subscribe).
    The solution described here is also known as SOME/IP-SD
    (Scalable service-Oriented MiddlewarE over IP - Service Discovery).
    
    .. seealso::
       Source - https://www.autosar.org/fileadmin/user_upload/standards/foundation/19-11/AUTOSAR_PRS_SOMEIPServiceDiscoveryProtocol.pdf
    """
    SEQ_FIELDS = ["flags", "reserved", "len_entries", "entries", "len_options", "options"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['flags']['start'] = self._io.pos()
        self.flags = SomeIpSd.SdFlags(self._io, self, self._root)
        self.flags._read()
        self._debug['flags']['end'] = self._io.pos()
        self._debug['reserved']['start'] = self._io.pos()
        self.reserved = self._io.read_bytes(3)
        self._debug['reserved']['end'] = self._io.pos()
        self._debug['len_entries']['start'] = self._io.pos()
        self.len_entries = self._io.read_u4be()
        self._debug['len_entries']['end'] = self._io.pos()
        self._debug['entries']['start'] = self._io.pos()
        self._raw_entries = self._io.read_bytes(self.len_entries)
        _io__raw_entries = KaitaiStream(BytesIO(self._raw_entries))
        self.entries = some_ip_sd_entries.SomeIpSdEntries(_io__raw_entries)
        self.entries._read()
        self._debug['entries']['end'] = self._io.pos()
        self._debug['len_options']['start'] = self._io.pos()
        self.len_options = self._io.read_u4be()
        self._debug['len_options']['end'] = self._io.pos()
        self._debug['options']['start'] = self._io.pos()
        self._raw_options = self._io.read_bytes(self.len_options)
        _io__raw_options = KaitaiStream(BytesIO(self._raw_options))
        self.options = some_ip_sd_options.SomeIpSdOptions(_io__raw_options)
        self.options._read()
        self._debug['options']['end'] = self._io.pos()

    class SdFlags(KaitaiStruct):
        """
        .. seealso::
           AUTOSAR_PRS_SOMEIPServiceDiscoveryProtocol.pdf - Figure 4.3
        """
        SEQ_FIELDS = ["reboot", "unicast", "initial_data", "reserved"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['reboot']['start'] = self._io.pos()
            self.reboot = self._io.read_bits_int_be(1) != 0
            self._debug['reboot']['end'] = self._io.pos()
            self._debug['unicast']['start'] = self._io.pos()
            self.unicast = self._io.read_bits_int_be(1) != 0
            self._debug['unicast']['end'] = self._io.pos()
            self._debug['initial_data']['start'] = self._io.pos()
            self.initial_data = self._io.read_bits_int_be(1) != 0
            self._debug['initial_data']['end'] = self._io.pos()
            self._debug['reserved']['start'] = self._io.pos()
            self.reserved = self._io.read_bits_int_be(5)
            self._debug['reserved']['end'] = self._io.pos()



