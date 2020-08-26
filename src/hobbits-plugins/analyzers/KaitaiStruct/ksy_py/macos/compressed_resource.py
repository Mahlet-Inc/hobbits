# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
import collections


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

import bytes_with_io
class CompressedResource(KaitaiStruct):
    """Compressed Macintosh resource data,
    as stored in resources with the "compressed" attribute.
    
    Resource decompression is not documented by Apple.
    It is mostly used internally in System 7,
    some of Apple's own applications (such as ResEdit),
    and also by some third-party applications.
    Later versions of Classic Mac OS make less use of resource compression,
    but still support it fully for backwards compatibility.
    Carbon in Mac OS X no longer supports resource compression in any way.
    
    The data of all compressed resources starts with a common header,
    followed by the compressed data.
    The data is decompressed using code in a `'dcmp'` resource.
    Some decompressors used by Apple are included in the System file,
    but applications can also include custom decompressors.
    The header of the compressed data indicates the ID of the `'dcmp'` resource used to decompress the data,
    along with some parameters for the decompressor.
    
    .. seealso::
       Source - http://www.alysis.us/arctechnology.htm
    
    
    .. seealso::
       Source - http://preserve.mactech.com/articles/mactech/Vol.09/09.01/ResCompression/index.html
    
    
    .. seealso::
       Source - https://github.com/dgelessus/python-rsrcfork/tree/master/rsrcfork/compress
    """
    SEQ_FIELDS = ["header", "compressed_data"]
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
        self._debug['compressed_data']['start'] = self._io.pos()
        self.compressed_data = self._io.read_bytes_full()
        self._debug['compressed_data']['end'] = self._io.pos()

    class Header(KaitaiStruct):
        """Compressed resource data header,
        as stored at the start of all compressed resources.
        """
        SEQ_FIELDS = ["common_part", "type_specific_part_raw_with_io"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['common_part']['start'] = self._io.pos()
            self.common_part = self._root.Header.CommonPart(self._io, self, self._root)
            self.common_part._read()
            self._debug['common_part']['end'] = self._io.pos()
            self._debug['type_specific_part_raw_with_io']['start'] = self._io.pos()
            self._raw_type_specific_part_raw_with_io = self._io.read_bytes((self.common_part.len_header - 12))
            _io__raw_type_specific_part_raw_with_io = KaitaiStream(BytesIO(self._raw_type_specific_part_raw_with_io))
            self.type_specific_part_raw_with_io = bytes_with_io.BytesWithIo(_io__raw_type_specific_part_raw_with_io)
            self.type_specific_part_raw_with_io._read()
            self._debug['type_specific_part_raw_with_io']['end'] = self._io.pos()

        class CommonPart(KaitaiStruct):
            """The common part of a compressed resource data header.
            The format of this part is the same for all compressed resources.
            """
            SEQ_FIELDS = ["magic", "len_header", "header_type", "unknown", "len_decompressed"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['magic']['start'] = self._io.pos()
                self.magic = self._io.read_bytes(4)
                self._debug['magic']['end'] = self._io.pos()
                if not self.magic == b"\xA8\x9F\x65\x72":
                    raise kaitaistruct.ValidationNotEqualError(b"\xA8\x9F\x65\x72", self.magic, self._io, u"/types/header/types/common_part/seq/0")
                self._debug['len_header']['start'] = self._io.pos()
                self.len_header = self._io.read_u2be()
                self._debug['len_header']['end'] = self._io.pos()
                if not self.len_header == 18:
                    raise kaitaistruct.ValidationNotEqualError(18, self.len_header, self._io, u"/types/header/types/common_part/seq/1")
                self._debug['header_type']['start'] = self._io.pos()
                self.header_type = self._io.read_u1()
                self._debug['header_type']['end'] = self._io.pos()
                self._debug['unknown']['start'] = self._io.pos()
                self.unknown = self._io.read_u1()
                self._debug['unknown']['end'] = self._io.pos()
                if not self.unknown == 1:
                    raise kaitaistruct.ValidationNotEqualError(1, self.unknown, self._io, u"/types/header/types/common_part/seq/3")
                self._debug['len_decompressed']['start'] = self._io.pos()
                self.len_decompressed = self._io.read_u4be()
                self._debug['len_decompressed']['end'] = self._io.pos()


        class TypeSpecificPartType8(KaitaiStruct):
            """The type-specific part of a compressed resource header with header type `8`.
            """
            SEQ_FIELDS = ["working_buffer_fractional_size", "expansion_buffer_size", "decompressor_id", "reserved"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['working_buffer_fractional_size']['start'] = self._io.pos()
                self.working_buffer_fractional_size = self._io.read_u1()
                self._debug['working_buffer_fractional_size']['end'] = self._io.pos()
                self._debug['expansion_buffer_size']['start'] = self._io.pos()
                self.expansion_buffer_size = self._io.read_u1()
                self._debug['expansion_buffer_size']['end'] = self._io.pos()
                self._debug['decompressor_id']['start'] = self._io.pos()
                self.decompressor_id = self._io.read_s2be()
                self._debug['decompressor_id']['end'] = self._io.pos()
                self._debug['reserved']['start'] = self._io.pos()
                self.reserved = self._io.read_u2be()
                self._debug['reserved']['end'] = self._io.pos()
                if not self.reserved == 0:
                    raise kaitaistruct.ValidationNotEqualError(0, self.reserved, self._io, u"/types/header/types/type_specific_part_type_8/seq/3")


        class TypeSpecificPartType9(KaitaiStruct):
            """The type-specific part of a compressed resource header with header type `9`.
            """
            SEQ_FIELDS = ["decompressor_id", "decompressor_specific_parameters_with_io"]
            def __init__(self, _io, _parent=None, _root=None):
                self._io = _io
                self._parent = _parent
                self._root = _root if _root else self
                self._debug = collections.defaultdict(dict)

            def _read(self):
                self._debug['decompressor_id']['start'] = self._io.pos()
                self.decompressor_id = self._io.read_s2be()
                self._debug['decompressor_id']['end'] = self._io.pos()
                self._debug['decompressor_specific_parameters_with_io']['start'] = self._io.pos()
                self._raw_decompressor_specific_parameters_with_io = self._io.read_bytes(4)
                _io__raw_decompressor_specific_parameters_with_io = KaitaiStream(BytesIO(self._raw_decompressor_specific_parameters_with_io))
                self.decompressor_specific_parameters_with_io = bytes_with_io.BytesWithIo(_io__raw_decompressor_specific_parameters_with_io)
                self.decompressor_specific_parameters_with_io._read()
                self._debug['decompressor_specific_parameters_with_io']['end'] = self._io.pos()

            @property
            def decompressor_specific_parameters(self):
                """Decompressor-specific parameters.
                The exact structure and meaning of this field is different for each decompressor.
                
                This field always has the same length,
                but decompressors don't always use the entirety of the field,
                so depending on the decompressor some parts of this field may be meaningless.
                """
                if hasattr(self, '_m_decompressor_specific_parameters'):
                    return self._m_decompressor_specific_parameters if hasattr(self, '_m_decompressor_specific_parameters') else None

                self._m_decompressor_specific_parameters = self.decompressor_specific_parameters_with_io.data
                return self._m_decompressor_specific_parameters if hasattr(self, '_m_decompressor_specific_parameters') else None


        @property
        def type_specific_part_raw(self):
            """The type-specific part of the header,
            as a raw byte array.
            """
            if hasattr(self, '_m_type_specific_part_raw'):
                return self._m_type_specific_part_raw if hasattr(self, '_m_type_specific_part_raw') else None

            self._m_type_specific_part_raw = self.type_specific_part_raw_with_io.data
            return self._m_type_specific_part_raw if hasattr(self, '_m_type_specific_part_raw') else None

        @property
        def type_specific_part(self):
            """The type-specific part of the header,
            parsed according to the type from the common part.
            """
            if hasattr(self, '_m_type_specific_part'):
                return self._m_type_specific_part if hasattr(self, '_m_type_specific_part') else None

            io = self.type_specific_part_raw_with_io._io
            _pos = io.pos()
            io.seek(0)
            self._debug['_m_type_specific_part']['start'] = io.pos()
            _on = self.common_part.header_type
            if _on == 8:
                self._m_type_specific_part = self._root.Header.TypeSpecificPartType8(io, self, self._root)
                self._m_type_specific_part._read()
            elif _on == 9:
                self._m_type_specific_part = self._root.Header.TypeSpecificPartType9(io, self, self._root)
                self._m_type_specific_part._read()
            self._debug['_m_type_specific_part']['end'] = io.pos()
            io.seek(_pos)
            return self._m_type_specific_part if hasattr(self, '_m_type_specific_part') else None



