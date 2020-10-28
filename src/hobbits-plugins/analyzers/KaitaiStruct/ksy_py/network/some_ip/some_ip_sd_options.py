# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
import collections
from enum import Enum


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class SomeIpSdOptions(KaitaiStruct):
    """FormatOptions are used to transport additional information to the entries. 
    This includes forinstance the information how a service instance is 
    reachable (IP-Address, TransportProtocol, Port Number).
    
    .. seealso::
       section 4.1.2.4 Options Format - https://www.autosar.org/fileadmin/user_upload/standards/foundation/19-11/AUTOSAR_PRS_SOMEIPServiceDiscoveryProtocol.pdf
       -
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
            _t_entries = SomeIpSdOptions.SdOption(self._io, self, self._root)
            _t_entries._read()
            self.entries.append(_t_entries)
            self._debug['entries']['arr'][len(self.entries) - 1]['end'] = self._io.pos()
            i += 1

        self._debug['entries']['end'] = self._io.pos()

    class SdOption(KaitaiStruct):

        class OptionTypes(Enum):
            configuration_option = 1
            load_balancing_option = 2
            ipv4_endpoint_option = 4
            ipv6_endpoint_option = 6
            ipv4_multicast_option = 20
            ipv6_multicast_option = 22
            ipv4_sd_endpoint_option = 36
            ipv6_sd_endpoint_option = 38
        SEQ_FIELDS = ["header", "content"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['header']['start'] = self._io.pos()
            self.header = SomeIpSdOptions.SdOption.SdOptionHeader(self._io, self, self._root)
            self.header._read()
            self._debug['header']['end'] = self._io.pos()
            self._debug['content']['start'] = self._io.pos()
            _on = self.header.type
            if _on == SomeIpSdOptions.SdOption.OptionTypes.load_balancing_option:
                self.content = SomeIpSdOptions.SdOption.SdLoadBalancingOption(self._io, self, self._root)
                self.content._read()
            elif _on == SomeIpSdOptions.SdOption.OptionTypes.configuration_option:
                self.content = SomeIpSdOptions.SdOption.SdConfigurationOption(self._io, self, self._root)
                self.content._read()
            elif _on == SomeIpSdOptions.SdOption.OptionTypes.ipv4_sd_endpoint_option:
                self.content = SomeIpSdOptions.SdOption.SdIpv4SdEndpointOption(self._io, self, self._root)
                self.content._read()
            elif _on == SomeIpSdOptions.SdOption.OptionTypes.ipv4_endpoint_option:
                self.content = SomeIpSdOptions.SdOption.SdIpv4EndpointOption(self._io, self, self._root)
                self.content._read()
            elif _on == SomeIpSdOptions.SdOption.OptionTypes.ipv6_sd_endpoint_option:
                self.content = SomeIpSdOptions.SdOption.SdIpv6SdEndpointOption(self._io, self, self._root)
                self.content._read()
            elif _on == SomeIpSdOptions.SdOption.OptionTypes.ipv4_multicast_option:
                self.content = SomeIpSdOptions.SdOption.SdIpv4MulticastOption(self._io, self, self._root)
                self.content._read()
            elif _on == SomeIpSdOptions.SdOption.OptionTypes.ipv6_endpoint_option:
                self.content = SomeIpSdOptions.SdOption.SdIpv6EndpointOption(self._io, self, self._root)
                self.content._read()
            elif _on == SomeIpSdOptions.SdOption.OptionTypes.ipv6_multicast_option:
                self.content = SomeIpSdOptions.SdOption.SdIpv6MulticastOption(self._io, self, self._root)
                self.content._read()
            self._debug['content']['end'] = self._io.pos()

        class SdOptionHeader(KaitaiStruct):
            SEQ_FIELDS = ["length", "type"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['length']['start'] = self._io.pos()
                self.length = self._io.read_u2be()
                self._debug['length']['end'] = self._io.pos()
                self._debug['type']['start'] = self._io.pos()
                self.type = KaitaiStream.resolve_enum(SomeIpSdOptions.SdOption.OptionTypes, self._io.read_u1())
                self._debug['type']['end'] = self._io.pos()


        class SdConfigString(KaitaiStruct):
            SEQ_FIELDS = ["length", "config"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['length']['start'] = self._io.pos()
                self.length = self._io.read_u1()
                self._debug['length']['end'] = self._io.pos()
                if self.length != 0:
                    self._debug['config']['start'] = self._io.pos()
                    self._raw_config = self._io.read_bytes(self.length)
                    _io__raw_config = KaitaiStream(BytesIO(self._raw_config))
                    self.config = SomeIpSdOptions.SdOption.SdConfigKvPair(_io__raw_config, self, self._root)
                    self.config._read()
                    self._debug['config']['end'] = self._io.pos()



        class SdConfigStringsContainer(KaitaiStruct):
            SEQ_FIELDS = ["config_strings"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['config_strings']['start'] = self._io.pos()
                self.config_strings = []
                i = 0
                while not self._io.is_eof():
                    if not 'arr' in self._debug['config_strings']:
                        self._debug['config_strings']['arr'] = []
                    self._debug['config_strings']['arr'].append({'start': self._io.pos()})
                    _t_config_strings = SomeIpSdOptions.SdOption.SdConfigString(self._io, self, self._root)
                    _t_config_strings._read()
                    self.config_strings.append(_t_config_strings)
                    self._debug['config_strings']['arr'][len(self.config_strings) - 1]['end'] = self._io.pos()
                    i += 1

                self._debug['config_strings']['end'] = self._io.pos()


        class SdConfigurationOption(KaitaiStruct):
            SEQ_FIELDS = ["reserved", "configurations"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['reserved']['start'] = self._io.pos()
                self.reserved = self._io.read_u1()
                self._debug['reserved']['end'] = self._io.pos()
                self._debug['configurations']['start'] = self._io.pos()
                self._raw_configurations = self._io.read_bytes((self._parent.header.length - 1))
                _io__raw_configurations = KaitaiStream(BytesIO(self._raw_configurations))
                self.configurations = SomeIpSdOptions.SdOption.SdConfigStringsContainer(_io__raw_configurations, self, self._root)
                self.configurations._read()
                self._debug['configurations']['end'] = self._io.pos()


        class SdIpv4MulticastOption(KaitaiStruct):
            SEQ_FIELDS = ["reserved", "address", "reserved2", "l4_protocol", "port"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['reserved']['start'] = self._io.pos()
                self.reserved = self._io.read_u1()
                self._debug['reserved']['end'] = self._io.pos()
                self._debug['address']['start'] = self._io.pos()
                self.address = self._io.read_bytes(4)
                self._debug['address']['end'] = self._io.pos()
                self._debug['reserved2']['start'] = self._io.pos()
                self.reserved2 = self._io.read_u1()
                self._debug['reserved2']['end'] = self._io.pos()
                self._debug['l4_protocol']['start'] = self._io.pos()
                self.l4_protocol = self._io.read_u1()
                self._debug['l4_protocol']['end'] = self._io.pos()
                self._debug['port']['start'] = self._io.pos()
                self.port = self._io.read_u2be()
                self._debug['port']['end'] = self._io.pos()


        class SdIpv4SdEndpointOption(KaitaiStruct):
            SEQ_FIELDS = ["reserved", "address", "reserved2", "l4_protocol", "port"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['reserved']['start'] = self._io.pos()
                self.reserved = self._io.read_u1()
                self._debug['reserved']['end'] = self._io.pos()
                self._debug['address']['start'] = self._io.pos()
                self.address = self._io.read_bytes(4)
                self._debug['address']['end'] = self._io.pos()
                self._debug['reserved2']['start'] = self._io.pos()
                self.reserved2 = self._io.read_u1()
                self._debug['reserved2']['end'] = self._io.pos()
                self._debug['l4_protocol']['start'] = self._io.pos()
                self.l4_protocol = self._io.read_u1()
                self._debug['l4_protocol']['end'] = self._io.pos()
                self._debug['port']['start'] = self._io.pos()
                self.port = self._io.read_u2be()
                self._debug['port']['end'] = self._io.pos()


        class SdIpv6MulticastOption(KaitaiStruct):
            SEQ_FIELDS = ["reserved", "address", "reserved2", "l4_protocol", "port"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['reserved']['start'] = self._io.pos()
                self.reserved = self._io.read_u1()
                self._debug['reserved']['end'] = self._io.pos()
                self._debug['address']['start'] = self._io.pos()
                self.address = self._io.read_bytes(16)
                self._debug['address']['end'] = self._io.pos()
                self._debug['reserved2']['start'] = self._io.pos()
                self.reserved2 = self._io.read_u1()
                self._debug['reserved2']['end'] = self._io.pos()
                self._debug['l4_protocol']['start'] = self._io.pos()
                self.l4_protocol = self._io.read_u1()
                self._debug['l4_protocol']['end'] = self._io.pos()
                self._debug['port']['start'] = self._io.pos()
                self.port = self._io.read_u2be()
                self._debug['port']['end'] = self._io.pos()


        class SdConfigKvPair(KaitaiStruct):
            SEQ_FIELDS = ["key", "value"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['key']['start'] = self._io.pos()
                self.key = (self._io.read_bytes_term(61, False, True, True)).decode(u"ASCII")
                self._debug['key']['end'] = self._io.pos()
                self._debug['value']['start'] = self._io.pos()
                self.value = (self._io.read_bytes_full()).decode(u"ASCII")
                self._debug['value']['end'] = self._io.pos()


        class SdIpv6SdEndpointOption(KaitaiStruct):
            SEQ_FIELDS = ["reserved", "address", "reserved2", "l4_protocol", "port"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['reserved']['start'] = self._io.pos()
                self.reserved = self._io.read_u1()
                self._debug['reserved']['end'] = self._io.pos()
                self._debug['address']['start'] = self._io.pos()
                self.address = self._io.read_bytes(16)
                self._debug['address']['end'] = self._io.pos()
                self._debug['reserved2']['start'] = self._io.pos()
                self.reserved2 = self._io.read_u1()
                self._debug['reserved2']['end'] = self._io.pos()
                self._debug['l4_protocol']['start'] = self._io.pos()
                self.l4_protocol = self._io.read_u1()
                self._debug['l4_protocol']['end'] = self._io.pos()
                self._debug['port']['start'] = self._io.pos()
                self.port = self._io.read_u2be()
                self._debug['port']['end'] = self._io.pos()


        class SdIpv4EndpointOption(KaitaiStruct):
            SEQ_FIELDS = ["reserved", "address", "reserved2", "l4_protocol", "port"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['reserved']['start'] = self._io.pos()
                self.reserved = self._io.read_u1()
                self._debug['reserved']['end'] = self._io.pos()
                self._debug['address']['start'] = self._io.pos()
                self.address = self._io.read_bytes(4)
                self._debug['address']['end'] = self._io.pos()
                self._debug['reserved2']['start'] = self._io.pos()
                self.reserved2 = self._io.read_u1()
                self._debug['reserved2']['end'] = self._io.pos()
                self._debug['l4_protocol']['start'] = self._io.pos()
                self.l4_protocol = self._io.read_u1()
                self._debug['l4_protocol']['end'] = self._io.pos()
                self._debug['port']['start'] = self._io.pos()
                self.port = self._io.read_u2be()
                self._debug['port']['end'] = self._io.pos()


        class SdIpv6EndpointOption(KaitaiStruct):
            SEQ_FIELDS = ["reserved", "address", "reserved2", "l4_protocol", "port"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['reserved']['start'] = self._io.pos()
                self.reserved = self._io.read_u1()
                self._debug['reserved']['end'] = self._io.pos()
                self._debug['address']['start'] = self._io.pos()
                self.address = self._io.read_bytes(16)
                self._debug['address']['end'] = self._io.pos()
                self._debug['reserved2']['start'] = self._io.pos()
                self.reserved2 = self._io.read_u1()
                self._debug['reserved2']['end'] = self._io.pos()
                self._debug['l4_protocol']['start'] = self._io.pos()
                self.l4_protocol = self._io.read_u1()
                self._debug['l4_protocol']['end'] = self._io.pos()
                self._debug['port']['start'] = self._io.pos()
                self.port = self._io.read_u2be()
                self._debug['port']['end'] = self._io.pos()


        class SdLoadBalancingOption(KaitaiStruct):
            SEQ_FIELDS = ["reserved", "priority", "weight"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['reserved']['start'] = self._io.pos()
                self.reserved = self._io.read_u1()
                self._debug['reserved']['end'] = self._io.pos()
                self._debug['priority']['start'] = self._io.pos()
                self.priority = self._io.read_u2be()
                self._debug['priority']['end'] = self._io.pos()
                self._debug['weight']['start'] = self._io.pos()
                self.weight = self._io.read_u2be()
                self._debug['weight']['end'] = self._io.pos()




