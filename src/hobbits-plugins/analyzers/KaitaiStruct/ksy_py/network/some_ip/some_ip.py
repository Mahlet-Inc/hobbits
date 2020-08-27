# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
import collections
from enum import Enum


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

import some_ip_sd
class SomeIp(KaitaiStruct):
    """SOME/IP (Scalable service-Oriented MiddlewarE over IP) is an automotive/embedded 
    communication protocol which supports remoteprocedure calls, event notifications 
    and the underlying serialization/wire format.
    
    .. seealso::
       Source - https://www.autosar.org/fileadmin/user_upload/standards/foundation/19-11/AUTOSAR_PRS_SOMEIPProtocol.pdf
    """
    SEQ_FIELDS = ["header", "payload"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['header']['start'] = self._io.pos()
        self.header = self._root.Header(self._io, self, self._root)
        self.header._read()
        self._debug['header']['end'] = self._io.pos()
        self._debug['payload']['start'] = self._io.pos()
        _on = self.header.message_id.value
        if _on == 4294934784:
            self._raw_payload = self._io.read_bytes((self.header.length - 8))
            _io__raw_payload = KaitaiStream(BytesIO(self._raw_payload))
            self.payload = some_ip_sd.SomeIpSd(_io__raw_payload)
            self.payload._read()
        else:
            self.payload = self._io.read_bytes((self.header.length - 8))
        self._debug['payload']['end'] = self._io.pos()

    class Header(KaitaiStruct):

        class MessageTypeEnum(Enum):
            request = 0
            request_no_return = 1
            notification = 2
            request_ack = 64
            request_no_return_ack = 65
            notification_ack = 66
            response = 128
            error = 129
            response_ack = 192
            error_ack = 193

        class ReturnCodeEnum(Enum):
            ok = 0
            not_ok = 1
            unknown_service = 2
            unknown_method = 3
            not_ready = 4
            not_reachable = 5
            time_out = 6
            wrong_protocol_version = 7
            wrong_interface_version = 8
            malformed_message = 9
            wrong_message_type = 10
        SEQ_FIELDS = ["message_id", "length", "request_id", "protocol_version", "interface_version", "message_type", "return_code"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['message_id']['start'] = self._io.pos()
            self._raw_message_id = self._io.read_bytes(4)
            _io__raw_message_id = KaitaiStream(BytesIO(self._raw_message_id))
            self.message_id = self._root.Header.MessageId(_io__raw_message_id, self, self._root)
            self.message_id._read()
            self._debug['message_id']['end'] = self._io.pos()
            self._debug['length']['start'] = self._io.pos()
            self.length = self._io.read_u4be()
            self._debug['length']['end'] = self._io.pos()
            self._debug['request_id']['start'] = self._io.pos()
            self._raw_request_id = self._io.read_bytes(4)
            _io__raw_request_id = KaitaiStream(BytesIO(self._raw_request_id))
            self.request_id = self._root.Header.RequestId(_io__raw_request_id, self, self._root)
            self.request_id._read()
            self._debug['request_id']['end'] = self._io.pos()
            self._debug['protocol_version']['start'] = self._io.pos()
            self.protocol_version = self._io.read_u1()
            self._debug['protocol_version']['end'] = self._io.pos()
            self._debug['interface_version']['start'] = self._io.pos()
            self.interface_version = self._io.read_u1()
            self._debug['interface_version']['end'] = self._io.pos()
            self._debug['message_type']['start'] = self._io.pos()
            self.message_type = KaitaiStream.resolve_enum(self._root.Header.MessageTypeEnum, self._io.read_u1())
            self._debug['message_type']['end'] = self._io.pos()
            self._debug['return_code']['start'] = self._io.pos()
            self.return_code = KaitaiStream.resolve_enum(self._root.Header.ReturnCodeEnum, self._io.read_u1())
            self._debug['return_code']['end'] = self._io.pos()

        class MessageId(KaitaiStruct):
            """[PRS_SOMEIP_00035] The assignment of the Message ID shall be up to 
            the user. However, the Message ID shall be unique for the whole 
            system (i.e. the vehicle). TheMessage ID is similar to a CAN ID and
            should be handled via a comparable process.
            [PRS_SOMEIP_00038] Message IDs of method calls shall be structured in
            the ID with 2^16 services with 2^15 methods.        
            
            .. seealso::
               AUTOSAR_PRS_SOMEIPProtocol.pdf 4.1.1.1  Message ID
            """
            SEQ_FIELDS = ["service_id", "sub_id", "method_id", "event_id"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['service_id']['start'] = self._io.pos()
                self.service_id = self._io.read_u2be()
                self._debug['service_id']['end'] = self._io.pos()
                self._debug['sub_id']['start'] = self._io.pos()
                self.sub_id = self._io.read_bits_int(1) != 0
                self._debug['sub_id']['end'] = self._io.pos()
                if self.sub_id == False:
                    self._debug['method_id']['start'] = self._io.pos()
                    self.method_id = self._io.read_bits_int(15)
                    self._debug['method_id']['end'] = self._io.pos()

                if self.sub_id == True:
                    self._debug['event_id']['start'] = self._io.pos()
                    self.event_id = self._io.read_bits_int(15)
                    self._debug['event_id']['end'] = self._io.pos()


            @property
            def value(self):
                """The value provides the undissected Message ID."""
                if hasattr(self, '_m_value'):
                    return self._m_value if hasattr(self, '_m_value') else None

                _pos = self._io.pos()
                self._io.seek(0)
                self._debug['_m_value']['start'] = self._io.pos()
                self._m_value = self._io.read_u4be()
                self._debug['_m_value']['end'] = self._io.pos()
                self._io.seek(_pos)
                return self._m_value if hasattr(self, '_m_value') else None


        class RequestId(KaitaiStruct):
            """The Request ID allows a provider and subscriber to differentiate 
            multiple parallel usesof the same method, event, getter or setter.
            
            .. seealso::
               AUTOSAR_PRS_SOMEIPProtocol.pdf - section 4.1.1.3  Request ID
            """
            SEQ_FIELDS = ["client_id", "session_id"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['client_id']['start'] = self._io.pos()
                self.client_id = self._io.read_u2be()
                self._debug['client_id']['end'] = self._io.pos()
                self._debug['session_id']['start'] = self._io.pos()
                self.session_id = self._io.read_u2be()
                self._debug['session_id']['end'] = self._io.pos()

            @property
            def value(self):
                """The value provides the undissected Request ID."""
                if hasattr(self, '_m_value'):
                    return self._m_value if hasattr(self, '_m_value') else None

                _pos = self._io.pos()
                self._io.seek(0)
                self._debug['_m_value']['start'] = self._io.pos()
                self._m_value = self._io.read_u4be()
                self._debug['_m_value']['end'] = self._io.pos()
                self._io.seek(_pos)
                return self._m_value if hasattr(self, '_m_value') else None


        @property
        def is_valid_service_discovery(self):
            """auxillary value.
            
            .. seealso::
               AUTOSAR_PRS_SOMEIPServiceDiscoveryProtocol.pdf - section 4.1.2.1 General Requirements
            """
            if hasattr(self, '_m_is_valid_service_discovery'):
                return self._m_is_valid_service_discovery if hasattr(self, '_m_is_valid_service_discovery') else None

            self._m_is_valid_service_discovery =  ((self.message_id.value == 4294934784) and (self.protocol_version == 1) and (self.interface_version == 1) and (self.message_type == self._root.Header.MessageTypeEnum.notification) and (self.return_code == self._root.Header.ReturnCodeEnum.ok)) 
            return self._m_is_valid_service_discovery if hasattr(self, '_m_is_valid_service_discovery') else None



