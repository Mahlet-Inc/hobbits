# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
import collections


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class Nitf(KaitaiStruct):
    """The NITF (National Image Transition Format) format is a file format developed by the U.S. Government for
    storing imagery, e.g. from satellites.
    
    According to the [foreword of the specification](https://gwg.nga.mil/ntb/baseline/docs/2500c/2500C.pdf):
    > The National Imagery Transmission Format Standard (NITFS) is the suite of standards for formatting digital
    > imagery and imagery-related products and exchanging them among members of the Intelligence Community (IC) as
    > defined by the Executive Order 12333, and other United States Government departments and agencies."
    
    This implementation is set to version format (`file_version`) of 02.10 and `standard_type` of `BF01`.
    It was implemented by [River Loop Security](https://riverloopsecurity.com).
    
    .. seealso::
       Source - https://gwg.nga.mil/ntb/baseline/docs/2500c/2500C.pdf
    """
    SEQ_FIELDS = ["header", "image_segments", "graphics_segments", "text_segments", "data_extension_segments", "reserved_extension_segments"]
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
        self._debug['image_segments']['start'] = self._io.pos()
        self.image_segments = [None] * (int(self.header.num_image_segments))
        for i in range(int(self.header.num_image_segments)):
            if not 'arr' in self._debug['image_segments']:
                self._debug['image_segments']['arr'] = []
            self._debug['image_segments']['arr'].append({'start': self._io.pos()})
            _t_image_segments = self._root.ImageSegment(i, self._io, self, self._root)
            _t_image_segments._read()
            self.image_segments[i] = _t_image_segments
            self._debug['image_segments']['arr'][i]['end'] = self._io.pos()

        self._debug['image_segments']['end'] = self._io.pos()
        self._debug['graphics_segments']['start'] = self._io.pos()
        self.graphics_segments = [None] * (int(self.header.num_graphics_segments))
        for i in range(int(self.header.num_graphics_segments)):
            if not 'arr' in self._debug['graphics_segments']:
                self._debug['graphics_segments']['arr'] = []
            self._debug['graphics_segments']['arr'].append({'start': self._io.pos()})
            _t_graphics_segments = self._root.GraphicsSegment(i, self._io, self, self._root)
            _t_graphics_segments._read()
            self.graphics_segments[i] = _t_graphics_segments
            self._debug['graphics_segments']['arr'][i]['end'] = self._io.pos()

        self._debug['graphics_segments']['end'] = self._io.pos()
        self._debug['text_segments']['start'] = self._io.pos()
        self.text_segments = [None] * (int(self.header.num_text_files))
        for i in range(int(self.header.num_text_files)):
            if not 'arr' in self._debug['text_segments']:
                self._debug['text_segments']['arr'] = []
            self._debug['text_segments']['arr'].append({'start': self._io.pos()})
            _t_text_segments = self._root.TextSegment(i, self._io, self, self._root)
            _t_text_segments._read()
            self.text_segments[i] = _t_text_segments
            self._debug['text_segments']['arr'][i]['end'] = self._io.pos()

        self._debug['text_segments']['end'] = self._io.pos()
        self._debug['data_extension_segments']['start'] = self._io.pos()
        self.data_extension_segments = [None] * (int(self.header.num_data_extension))
        for i in range(int(self.header.num_data_extension)):
            if not 'arr' in self._debug['data_extension_segments']:
                self._debug['data_extension_segments']['arr'] = []
            self._debug['data_extension_segments']['arr'].append({'start': self._io.pos()})
            _t_data_extension_segments = self._root.DataExtensionSegment(i, self._io, self, self._root)
            _t_data_extension_segments._read()
            self.data_extension_segments[i] = _t_data_extension_segments
            self._debug['data_extension_segments']['arr'][i]['end'] = self._io.pos()

        self._debug['data_extension_segments']['end'] = self._io.pos()
        self._debug['reserved_extension_segments']['start'] = self._io.pos()
        self.reserved_extension_segments = [None] * (int(self.header.num_reserved_extension))
        for i in range(int(self.header.num_reserved_extension)):
            if not 'arr' in self._debug['reserved_extension_segments']:
                self._debug['reserved_extension_segments']['arr'] = []
            self._debug['reserved_extension_segments']['arr'].append({'start': self._io.pos()})
            _t_reserved_extension_segments = self._root.ReservedExtensionSegment(i, self._io, self, self._root)
            _t_reserved_extension_segments._read()
            self.reserved_extension_segments[i] = _t_reserved_extension_segments
            self._debug['reserved_extension_segments']['arr'][i]['end'] = self._io.pos()

        self._debug['reserved_extension_segments']['end'] = self._io.pos()

    class ReservedExtensionSegment(KaitaiStruct):
        SEQ_FIELDS = ["reserved_sub_header", "reserved_data_field"]
        def __init__(self, idx, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self.idx = idx
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['reserved_sub_header']['start'] = self._io.pos()
            self._raw_reserved_sub_header = self._io.read_bytes(int(self._parent.header.lrnfo[self.idx].length_reserved_extension_subheader))
            _io__raw_reserved_sub_header = KaitaiStream(BytesIO(self._raw_reserved_sub_header))
            self.reserved_sub_header = self._root.ReservedSubHeader(_io__raw_reserved_sub_header, self, self._root)
            self.reserved_sub_header._read()
            self._debug['reserved_sub_header']['end'] = self._io.pos()
            self._debug['reserved_data_field']['start'] = self._io.pos()
            self.reserved_data_field = self._io.read_bytes(int(self._parent.header.lrnfo[self.idx].length_reserved_extension_segment))
            self._debug['reserved_data_field']['end'] = self._io.pos()


    class ImageComment(KaitaiStruct):
        SEQ_FIELDS = ["_unnamed0"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['_unnamed0']['start'] = self._io.pos()
            self._unnamed0 = (self._io.read_bytes(80)).decode(u"UTF-8")
            self._debug['_unnamed0']['end'] = self._io.pos()


    class LengthReservedInfo(KaitaiStruct):
        SEQ_FIELDS = ["length_reserved_extension_subheader", "length_reserved_extension_segment"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['length_reserved_extension_subheader']['start'] = self._io.pos()
            self.length_reserved_extension_subheader = (self._io.read_bytes(4)).decode(u"UTF-8")
            self._debug['length_reserved_extension_subheader']['end'] = self._io.pos()
            self._debug['length_reserved_extension_segment']['start'] = self._io.pos()
            self.length_reserved_extension_segment = (self._io.read_bytes(7)).decode(u"UTF-8")
            self._debug['length_reserved_extension_segment']['end'] = self._io.pos()


    class Tre(KaitaiStruct):
        SEQ_FIELDS = ["extension_type_id", "edata_length", "edata"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['extension_type_id']['start'] = self._io.pos()
            self.extension_type_id = (self._io.read_bytes(6)).decode(u"UTF-8")
            self._debug['extension_type_id']['end'] = self._io.pos()
            self._debug['edata_length']['start'] = self._io.pos()
            self.edata_length = (self._io.read_bytes(5)).decode(u"UTF-8")
            self._debug['edata_length']['end'] = self._io.pos()
            self._debug['edata']['start'] = self._io.pos()
            self.edata = (self._io.read_bytes(int(self.edata_length))).decode(u"UTF-8")
            self._debug['edata']['end'] = self._io.pos()


    class BandInfo(KaitaiStruct):
        SEQ_FIELDS = ["representation", "subcategory", "img_filter_condition", "img_filter_code", "num_luts", "num_lut_entries", "luts"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['representation']['start'] = self._io.pos()
            self.representation = (self._io.read_bytes(2)).decode(u"UTF-8")
            self._debug['representation']['end'] = self._io.pos()
            self._debug['subcategory']['start'] = self._io.pos()
            self.subcategory = (self._io.read_bytes(6)).decode(u"UTF-8")
            self._debug['subcategory']['end'] = self._io.pos()
            self._debug['img_filter_condition']['start'] = self._io.pos()
            self.img_filter_condition = self._io.read_bytes(1)
            self._debug['img_filter_condition']['end'] = self._io.pos()
            if not self.img_filter_condition == b"\x4E":
                raise kaitaistruct.ValidationNotEqualError(b"\x4E", self.img_filter_condition, self._io, u"/types/band_info/seq/2")
            self._debug['img_filter_code']['start'] = self._io.pos()
            self.img_filter_code = (self._io.read_bytes(3)).decode(u"UTF-8")
            self._debug['img_filter_code']['end'] = self._io.pos()
            self._debug['num_luts']['start'] = self._io.pos()
            self.num_luts = (self._io.read_bytes(1)).decode(u"UTF-8")
            self._debug['num_luts']['end'] = self._io.pos()
            if int(self.num_luts) != 0:
                self._debug['num_lut_entries']['start'] = self._io.pos()
                self.num_lut_entries = (self._io.read_bytes(5)).decode(u"UTF-8")
                self._debug['num_lut_entries']['end'] = self._io.pos()

            self._debug['luts']['start'] = self._io.pos()
            self.luts = [None] * (int(self.num_luts))
            for i in range(int(self.num_luts)):
                if not 'arr' in self._debug['luts']:
                    self._debug['luts']['arr'] = []
                self._debug['luts']['arr'].append({'start': self._io.pos()})
                self.luts[i] = self._io.read_bytes(int(self.num_lut_entries))
                self._debug['luts']['arr'][i]['end'] = self._io.pos()

            self._debug['luts']['end'] = self._io.pos()


    class ImageSegment(KaitaiStruct):
        SEQ_FIELDS = ["image_sub_header", "image_data_mask", "image_data_field"]
        def __init__(self, idx, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self.idx = idx
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['image_sub_header']['start'] = self._io.pos()
            self.image_sub_header = self._root.ImageSubHeader(self._io, self, self._root)
            self.image_sub_header._read()
            self._debug['image_sub_header']['end'] = self._io.pos()
            if self.has_mask:
                self._debug['image_data_mask']['start'] = self._io.pos()
                self.image_data_mask = self._root.ImageDataMask(self._io, self, self._root)
                self.image_data_mask._read()
                self._debug['image_data_mask']['end'] = self._io.pos()

            if self.has_mask:
                self._debug['image_data_field']['start'] = self._io.pos()
                self.image_data_field = self._io.read_bytes((int(self._parent.header.linfo[self.idx].length_image_segment) - self.image_data_mask.total_size))
                self._debug['image_data_field']['end'] = self._io.pos()


        @property
        def has_mask(self):
            if hasattr(self, '_m_has_mask'):
                return self._m_has_mask if hasattr(self, '_m_has_mask') else None

            self._m_has_mask =  ((self.image_sub_header.img_compression[0:1] == u"M") or (self.image_sub_header.img_compression[1:2] == u"M")) 
            return self._m_has_mask if hasattr(self, '_m_has_mask') else None


    class TextSegment(KaitaiStruct):
        SEQ_FIELDS = ["text_sub_header", "text_data_field"]
        def __init__(self, idx, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self.idx = idx
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['text_sub_header']['start'] = self._io.pos()
            self.text_sub_header = self._io.read_bytes(1)
            self._debug['text_sub_header']['end'] = self._io.pos()
            self._debug['text_data_field']['start'] = self._io.pos()
            self.text_data_field = self._io.read_bytes(int(self._parent.header.ltnfo[self.idx].length_text_segment))
            self._debug['text_data_field']['end'] = self._io.pos()


    class GraphicSubHeader(KaitaiStruct):
        SEQ_FIELDS = ["file_part_type_sy", "graphic_id", "graphic_name", "graphic_classification", "encryption", "graphic_type", "reserved1", "graphic_display_level", "graphic_attachment_level", "graphic_location", "first_graphic_bound_loc", "graphic_color", "second_graphic_bound_loc", "reserved2", "graphics_extended_sub_header"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['file_part_type_sy']['start'] = self._io.pos()
            self.file_part_type_sy = self._io.read_bytes(2)
            self._debug['file_part_type_sy']['end'] = self._io.pos()
            if not self.file_part_type_sy == b"\x53\x59":
                raise kaitaistruct.ValidationNotEqualError(b"\x53\x59", self.file_part_type_sy, self._io, u"/types/graphic_sub_header/seq/0")
            self._debug['graphic_id']['start'] = self._io.pos()
            self.graphic_id = (self._io.read_bytes(10)).decode(u"UTF-8")
            self._debug['graphic_id']['end'] = self._io.pos()
            self._debug['graphic_name']['start'] = self._io.pos()
            self.graphic_name = (self._io.read_bytes(20)).decode(u"UTF-8")
            self._debug['graphic_name']['end'] = self._io.pos()
            self._debug['graphic_classification']['start'] = self._io.pos()
            self.graphic_classification = self._root.Clasnfo(self._io, self, self._root)
            self.graphic_classification._read()
            self._debug['graphic_classification']['end'] = self._io.pos()
            self._debug['encryption']['start'] = self._io.pos()
            self.encryption = self._root.Encrypt(self._io, self, self._root)
            self.encryption._read()
            self._debug['encryption']['end'] = self._io.pos()
            self._debug['graphic_type']['start'] = self._io.pos()
            self.graphic_type = self._io.read_bytes(1)
            self._debug['graphic_type']['end'] = self._io.pos()
            if not self.graphic_type == b"\x43":
                raise kaitaistruct.ValidationNotEqualError(b"\x43", self.graphic_type, self._io, u"/types/graphic_sub_header/seq/5")
            self._debug['reserved1']['start'] = self._io.pos()
            self.reserved1 = (self._io.read_bytes(13)).decode(u"UTF-8")
            self._debug['reserved1']['end'] = self._io.pos()
            self._debug['graphic_display_level']['start'] = self._io.pos()
            self.graphic_display_level = (self._io.read_bytes(3)).decode(u"UTF-8")
            self._debug['graphic_display_level']['end'] = self._io.pos()
            self._debug['graphic_attachment_level']['start'] = self._io.pos()
            self.graphic_attachment_level = (self._io.read_bytes(3)).decode(u"UTF-8")
            self._debug['graphic_attachment_level']['end'] = self._io.pos()
            self._debug['graphic_location']['start'] = self._io.pos()
            self.graphic_location = (self._io.read_bytes(10)).decode(u"UTF-8")
            self._debug['graphic_location']['end'] = self._io.pos()
            self._debug['first_graphic_bound_loc']['start'] = self._io.pos()
            self.first_graphic_bound_loc = (self._io.read_bytes(10)).decode(u"UTF-8")
            self._debug['first_graphic_bound_loc']['end'] = self._io.pos()
            self._debug['graphic_color']['start'] = self._io.pos()
            self.graphic_color = (self._io.read_bytes(1)).decode(u"UTF-8")
            self._debug['graphic_color']['end'] = self._io.pos()
            self._debug['second_graphic_bound_loc']['start'] = self._io.pos()
            self.second_graphic_bound_loc = (self._io.read_bytes(10)).decode(u"UTF-8")
            self._debug['second_graphic_bound_loc']['end'] = self._io.pos()
            self._debug['reserved2']['start'] = self._io.pos()
            self.reserved2 = (self._io.read_bytes(2)).decode(u"UTF-8")
            self._debug['reserved2']['end'] = self._io.pos()
            self._debug['graphics_extended_sub_header']['start'] = self._io.pos()
            self.graphics_extended_sub_header = self._root.TreHeader(self._io, self, self._root)
            self.graphics_extended_sub_header._read()
            self._debug['graphics_extended_sub_header']['end'] = self._io.pos()


    class Clasnfo(KaitaiStruct):
        SEQ_FIELDS = ["security_class", "security_system", "codewords", "control_and_handling", "releaseability", "declass_type", "declass_date", "declass_exemption", "downgrade", "downgrade_date", "class_text", "class_authority_type", "class_authority", "class_reason", "source_date", "control_number"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['security_class']['start'] = self._io.pos()
            self.security_class = (self._io.read_bytes(1)).decode(u"UTF-8")
            self._debug['security_class']['end'] = self._io.pos()
            self._debug['security_system']['start'] = self._io.pos()
            self.security_system = (self._io.read_bytes(2)).decode(u"UTF-8")
            self._debug['security_system']['end'] = self._io.pos()
            self._debug['codewords']['start'] = self._io.pos()
            self.codewords = (self._io.read_bytes(11)).decode(u"UTF-8")
            self._debug['codewords']['end'] = self._io.pos()
            self._debug['control_and_handling']['start'] = self._io.pos()
            self.control_and_handling = (self._io.read_bytes(2)).decode(u"UTF-8")
            self._debug['control_and_handling']['end'] = self._io.pos()
            self._debug['releaseability']['start'] = self._io.pos()
            self.releaseability = (self._io.read_bytes(20)).decode(u"UTF-8")
            self._debug['releaseability']['end'] = self._io.pos()
            self._debug['declass_type']['start'] = self._io.pos()
            self.declass_type = (self._io.read_bytes(2)).decode(u"UTF-8")
            self._debug['declass_type']['end'] = self._io.pos()
            self._debug['declass_date']['start'] = self._io.pos()
            self.declass_date = (self._io.read_bytes(8)).decode(u"UTF-8")
            self._debug['declass_date']['end'] = self._io.pos()
            self._debug['declass_exemption']['start'] = self._io.pos()
            self.declass_exemption = (self._io.read_bytes(4)).decode(u"UTF-8")
            self._debug['declass_exemption']['end'] = self._io.pos()
            self._debug['downgrade']['start'] = self._io.pos()
            self.downgrade = (self._io.read_bytes(1)).decode(u"UTF-8")
            self._debug['downgrade']['end'] = self._io.pos()
            self._debug['downgrade_date']['start'] = self._io.pos()
            self.downgrade_date = (self._io.read_bytes(8)).decode(u"UTF-8")
            self._debug['downgrade_date']['end'] = self._io.pos()
            self._debug['class_text']['start'] = self._io.pos()
            self.class_text = (self._io.read_bytes(43)).decode(u"UTF-8")
            self._debug['class_text']['end'] = self._io.pos()
            self._debug['class_authority_type']['start'] = self._io.pos()
            self.class_authority_type = (self._io.read_bytes(1)).decode(u"UTF-8")
            self._debug['class_authority_type']['end'] = self._io.pos()
            self._debug['class_authority']['start'] = self._io.pos()
            self.class_authority = (self._io.read_bytes(40)).decode(u"UTF-8")
            self._debug['class_authority']['end'] = self._io.pos()
            self._debug['class_reason']['start'] = self._io.pos()
            self.class_reason = (self._io.read_bytes(1)).decode(u"UTF-8")
            self._debug['class_reason']['end'] = self._io.pos()
            self._debug['source_date']['start'] = self._io.pos()
            self.source_date = (self._io.read_bytes(8)).decode(u"UTF-8")
            self._debug['source_date']['end'] = self._io.pos()
            self._debug['control_number']['start'] = self._io.pos()
            self.control_number = (self._io.read_bytes(15)).decode(u"UTF-8")
            self._debug['control_number']['end'] = self._io.pos()


    class LengthGraphicInfo(KaitaiStruct):
        SEQ_FIELDS = ["length_graphic_subheader", "length_graphic_segment"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['length_graphic_subheader']['start'] = self._io.pos()
            self.length_graphic_subheader = (self._io.read_bytes(4)).decode(u"UTF-8")
            self._debug['length_graphic_subheader']['end'] = self._io.pos()
            self._debug['length_graphic_segment']['start'] = self._io.pos()
            self.length_graphic_segment = (self._io.read_bytes(6)).decode(u"UTF-8")
            self._debug['length_graphic_segment']['end'] = self._io.pos()


    class Encrypt(KaitaiStruct):
        SEQ_FIELDS = ["_unnamed0"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['_unnamed0']['start'] = self._io.pos()
            self._unnamed0 = (self._io.read_bytes(1)).decode(u"UTF-8")
            self._debug['_unnamed0']['end'] = self._io.pos()


    class ImageDataMask(KaitaiStruct):
        SEQ_FIELDS = ["blocked_img_data_offset", "bmrlnth", "tmrlnth", "tpxcdlnth", "tpxcd", "bmrbnd", "tmrbnd"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['blocked_img_data_offset']['start'] = self._io.pos()
            self.blocked_img_data_offset = self._io.read_u4be()
            self._debug['blocked_img_data_offset']['end'] = self._io.pos()
            self._debug['bmrlnth']['start'] = self._io.pos()
            self.bmrlnth = self._io.read_u2be()
            self._debug['bmrlnth']['end'] = self._io.pos()
            self._debug['tmrlnth']['start'] = self._io.pos()
            self.tmrlnth = self._io.read_u2be()
            self._debug['tmrlnth']['end'] = self._io.pos()
            self._debug['tpxcdlnth']['start'] = self._io.pos()
            self.tpxcdlnth = self._io.read_u2be()
            self._debug['tpxcdlnth']['end'] = self._io.pos()
            self._debug['tpxcd']['start'] = self._io.pos()
            self.tpxcd = self._io.read_bytes(self.tpxcd_size)
            self._debug['tpxcd']['end'] = self._io.pos()
            if self.has_bmr:
                self._debug['bmrbnd']['start'] = self._io.pos()
                self.bmrbnd = [None] * (self.bmrtmr_count)
                for i in range(self.bmrtmr_count):
                    if not 'arr' in self._debug['bmrbnd']:
                        self._debug['bmrbnd']['arr'] = []
                    self._debug['bmrbnd']['arr'].append({'start': self._io.pos()})
                    self.bmrbnd[i] = self._io.read_u4be()
                    self._debug['bmrbnd']['arr'][i]['end'] = self._io.pos()

                self._debug['bmrbnd']['end'] = self._io.pos()

            if self.has_tmr:
                self._debug['tmrbnd']['start'] = self._io.pos()
                self.tmrbnd = [None] * (self.bmrtmr_count)
                for i in range(self.bmrtmr_count):
                    if not 'arr' in self._debug['tmrbnd']:
                        self._debug['tmrbnd']['arr'] = []
                    self._debug['tmrbnd']['arr'].append({'start': self._io.pos()})
                    self.tmrbnd[i] = self._io.read_u4be()
                    self._debug['tmrbnd']['arr'][i]['end'] = self._io.pos()

                self._debug['tmrbnd']['end'] = self._io.pos()


        @property
        def has_bmr(self):
            if hasattr(self, '_m_has_bmr'):
                return self._m_has_bmr if hasattr(self, '_m_has_bmr') else None

            self._m_has_bmr = self.bmrlnth != 0
            return self._m_has_bmr if hasattr(self, '_m_has_bmr') else None

        @property
        def has_tmr(self):
            if hasattr(self, '_m_has_tmr'):
                return self._m_has_tmr if hasattr(self, '_m_has_tmr') else None

            self._m_has_tmr = self.tmrlnth != 0
            return self._m_has_tmr if hasattr(self, '_m_has_tmr') else None

        @property
        def tmrbnd_size(self):
            if hasattr(self, '_m_tmrbnd_size'):
                return self._m_tmrbnd_size if hasattr(self, '_m_tmrbnd_size') else None

            self._m_tmrbnd_size = ((self.bmrtmr_count * 4) if self.has_tmr else 0)
            return self._m_tmrbnd_size if hasattr(self, '_m_tmrbnd_size') else None

        @property
        def tpxcd_size(self):
            if hasattr(self, '_m_tpxcd_size'):
                return self._m_tpxcd_size if hasattr(self, '_m_tpxcd_size') else None

            self._m_tpxcd_size = (self.tpxcdlnth if (self.tpxcdlnth % 8) == 0 else (self.tpxcdlnth + (8 - (self.tpxcdlnth % 8)))) // 8
            return self._m_tpxcd_size if hasattr(self, '_m_tpxcd_size') else None

        @property
        def total_size(self):
            if hasattr(self, '_m_total_size'):
                return self._m_total_size if hasattr(self, '_m_total_size') else None

            self._m_total_size = ((((((4 + 2) + 2) + 2) + self.tpxcd_size) + self.bmrbnd_size) + self.tmrbnd_size)
            return self._m_total_size if hasattr(self, '_m_total_size') else None

        @property
        def bmrbnd_size(self):
            if hasattr(self, '_m_bmrbnd_size'):
                return self._m_bmrbnd_size if hasattr(self, '_m_bmrbnd_size') else None

            self._m_bmrbnd_size = ((self.bmrtmr_count * 4) if self.has_bmr else 0)
            return self._m_bmrbnd_size if hasattr(self, '_m_bmrbnd_size') else None

        @property
        def bmrtmr_count(self):
            if hasattr(self, '_m_bmrtmr_count'):
                return self._m_bmrtmr_count if hasattr(self, '_m_bmrtmr_count') else None

            self._m_bmrtmr_count = ((int(self._parent.image_sub_header.num_blocks_per_row) * int(self._parent.image_sub_header.num_blocks_per_col)) * (1 if self._parent.image_sub_header.img_mode != u"S" else (int(self._parent.image_sub_header.num_bands) if int(self._parent.image_sub_header.num_bands) != 0 else int(self._parent.image_sub_header.num_multispectral_bands))))
            return self._m_bmrtmr_count if hasattr(self, '_m_bmrtmr_count') else None


    class GraphicsSegment(KaitaiStruct):
        SEQ_FIELDS = ["graphic_sub_header", "graphic_data_field"]
        def __init__(self, idx, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self.idx = idx
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['graphic_sub_header']['start'] = self._io.pos()
            self.graphic_sub_header = self._root.GraphicSubHeader(self._io, self, self._root)
            self.graphic_sub_header._read()
            self._debug['graphic_sub_header']['end'] = self._io.pos()
            self._debug['graphic_data_field']['start'] = self._io.pos()
            self.graphic_data_field = self._io.read_bytes(int(self._parent.header.lnnfo[self.idx].length_graphic_segment))
            self._debug['graphic_data_field']['end'] = self._io.pos()


    class DataSubHeader(KaitaiStruct):
        SEQ_FIELDS = ["des_base", "overflowed_header_type", "data_item_overflowed", "des_defined_subheader_fields_len", "desshf", "des_defined_data_field"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['des_base']['start'] = self._io.pos()
            self.des_base = self._root.DataSubHeaderBase(self._io, self, self._root)
            self.des_base._read()
            self._debug['des_base']['end'] = self._io.pos()
            if self.tre_ofl:
                self._debug['overflowed_header_type']['start'] = self._io.pos()
                self.overflowed_header_type = (self._io.read_bytes(6)).decode(u"UTF-8")
                self._debug['overflowed_header_type']['end'] = self._io.pos()

            if self.tre_ofl:
                self._debug['data_item_overflowed']['start'] = self._io.pos()
                self.data_item_overflowed = (self._io.read_bytes(3)).decode(u"UTF-8")
                self._debug['data_item_overflowed']['end'] = self._io.pos()

            self._debug['des_defined_subheader_fields_len']['start'] = self._io.pos()
            self.des_defined_subheader_fields_len = (self._io.read_bytes(4)).decode(u"UTF-8")
            self._debug['des_defined_subheader_fields_len']['end'] = self._io.pos()
            self._debug['desshf']['start'] = self._io.pos()
            self.desshf = (self._io.read_bytes(int(self.des_defined_subheader_fields_len))).decode(u"UTF-8")
            self._debug['desshf']['end'] = self._io.pos()
            self._debug['des_defined_data_field']['start'] = self._io.pos()
            self.des_defined_data_field = (self._io.read_bytes_full()).decode(u"UTF-8")
            self._debug['des_defined_data_field']['end'] = self._io.pos()

        @property
        def tre_ofl(self):
            if hasattr(self, '_m_tre_ofl'):
                return self._m_tre_ofl if hasattr(self, '_m_tre_ofl') else None

            self._m_tre_ofl = self.des_base.desid == u"TRE_OVERFLOW"
            return self._m_tre_ofl if hasattr(self, '_m_tre_ofl') else None


    class DataExtensionSegment(KaitaiStruct):
        SEQ_FIELDS = ["data_sub_header", "data_data_field"]
        def __init__(self, idx, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self.idx = idx
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['data_sub_header']['start'] = self._io.pos()
            self._raw_data_sub_header = self._io.read_bytes(int(self._parent.header.ldnfo[self.idx].length_data_extension_subheader))
            _io__raw_data_sub_header = KaitaiStream(BytesIO(self._raw_data_sub_header))
            self.data_sub_header = self._root.DataSubHeader(_io__raw_data_sub_header, self, self._root)
            self.data_sub_header._read()
            self._debug['data_sub_header']['end'] = self._io.pos()
            self._debug['data_data_field']['start'] = self._io.pos()
            self.data_data_field = self._io.read_bytes(int(self._parent.header.ldnfo[self.idx].length_data_extension_segment))
            self._debug['data_data_field']['end'] = self._io.pos()


    class DataSubHeaderTre(KaitaiStruct):
        SEQ_FIELDS = ["des_base", "overflowed_header_type", "data_item_overflowed", "des_defined_subheader_fields_len", "des_defined_data_field"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['des_base']['start'] = self._io.pos()
            self.des_base = self._root.DataSubHeaderBase(self._io, self, self._root)
            self.des_base._read()
            self._debug['des_base']['end'] = self._io.pos()
            if self.des_base.desid == u"TRE_OVERFLOW":
                self._debug['overflowed_header_type']['start'] = self._io.pos()
                self.overflowed_header_type = (self._io.read_bytes(6)).decode(u"UTF-8")
                self._debug['overflowed_header_type']['end'] = self._io.pos()

            if self.des_base.desid == u"TRE_OVERFLOW":
                self._debug['data_item_overflowed']['start'] = self._io.pos()
                self.data_item_overflowed = (self._io.read_bytes(3)).decode(u"UTF-8")
                self._debug['data_item_overflowed']['end'] = self._io.pos()

            self._debug['des_defined_subheader_fields_len']['start'] = self._io.pos()
            self.des_defined_subheader_fields_len = (self._io.read_bytes(4)).decode(u"UTF-8")
            self._debug['des_defined_subheader_fields_len']['end'] = self._io.pos()
            self._debug['des_defined_data_field']['start'] = self._io.pos()
            self.des_defined_data_field = (self._io.read_bytes(int(self.des_defined_subheader_fields_len))).decode(u"UTF-8")
            self._debug['des_defined_data_field']['end'] = self._io.pos()


    class ImageSubHeader(KaitaiStruct):
        SEQ_FIELDS = ["file_part_type", "image_id_1", "image_date_time", "target_id", "image_id_2", "image_security_classification", "encryption", "image_source", "num_sig_rows", "num_sig_cols", "pixel_value_type", "image_representation", "image_category", "actual_bits_per_pixel_per_band", "pixel_justification", "image_coordinate_rep", "image_geo_loc", "num_img_comments", "img_comments", "img_compression", "compression_rate_code", "num_bands", "num_multispectral_bands", "bands", "img_sync_code", "img_mode", "num_blocks_per_row", "num_blocks_per_col", "num_pixels_per_block_horz", "num_pixels_per_block_vert", "num_pixels_per_band", "img_display_level", "attachment_level", "img_location", "img_magnification", "user_def_img_data_len", "user_def_overflow", "user_def_img_data", "image_extended_sub_header"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['file_part_type']['start'] = self._io.pos()
            self.file_part_type = self._io.read_bytes(2)
            self._debug['file_part_type']['end'] = self._io.pos()
            if not self.file_part_type == b"\x49\x4D":
                raise kaitaistruct.ValidationNotEqualError(b"\x49\x4D", self.file_part_type, self._io, u"/types/image_sub_header/seq/0")
            self._debug['image_id_1']['start'] = self._io.pos()
            self.image_id_1 = (self._io.read_bytes(10)).decode(u"UTF-8")
            self._debug['image_id_1']['end'] = self._io.pos()
            self._debug['image_date_time']['start'] = self._io.pos()
            self.image_date_time = self._root.DateTime(self._io, self, self._root)
            self.image_date_time._read()
            self._debug['image_date_time']['end'] = self._io.pos()
            self._debug['target_id']['start'] = self._io.pos()
            self.target_id = (self._io.read_bytes(17)).decode(u"UTF-8")
            self._debug['target_id']['end'] = self._io.pos()
            self._debug['image_id_2']['start'] = self._io.pos()
            self.image_id_2 = (self._io.read_bytes(80)).decode(u"UTF-8")
            self._debug['image_id_2']['end'] = self._io.pos()
            self._debug['image_security_classification']['start'] = self._io.pos()
            self.image_security_classification = self._root.Clasnfo(self._io, self, self._root)
            self.image_security_classification._read()
            self._debug['image_security_classification']['end'] = self._io.pos()
            self._debug['encryption']['start'] = self._io.pos()
            self.encryption = self._root.Encrypt(self._io, self, self._root)
            self.encryption._read()
            self._debug['encryption']['end'] = self._io.pos()
            self._debug['image_source']['start'] = self._io.pos()
            self.image_source = (self._io.read_bytes(42)).decode(u"UTF-8")
            self._debug['image_source']['end'] = self._io.pos()
            self._debug['num_sig_rows']['start'] = self._io.pos()
            self.num_sig_rows = (self._io.read_bytes(8)).decode(u"UTF-8")
            self._debug['num_sig_rows']['end'] = self._io.pos()
            self._debug['num_sig_cols']['start'] = self._io.pos()
            self.num_sig_cols = (self._io.read_bytes(8)).decode(u"UTF-8")
            self._debug['num_sig_cols']['end'] = self._io.pos()
            self._debug['pixel_value_type']['start'] = self._io.pos()
            self.pixel_value_type = (self._io.read_bytes(3)).decode(u"UTF-8")
            self._debug['pixel_value_type']['end'] = self._io.pos()
            self._debug['image_representation']['start'] = self._io.pos()
            self.image_representation = (self._io.read_bytes(8)).decode(u"UTF-8")
            self._debug['image_representation']['end'] = self._io.pos()
            self._debug['image_category']['start'] = self._io.pos()
            self.image_category = (self._io.read_bytes(8)).decode(u"UTF-8")
            self._debug['image_category']['end'] = self._io.pos()
            self._debug['actual_bits_per_pixel_per_band']['start'] = self._io.pos()
            self.actual_bits_per_pixel_per_band = (self._io.read_bytes(2)).decode(u"UTF-8")
            self._debug['actual_bits_per_pixel_per_band']['end'] = self._io.pos()
            self._debug['pixel_justification']['start'] = self._io.pos()
            self.pixel_justification = (self._io.read_bytes(1)).decode(u"UTF-8")
            self._debug['pixel_justification']['end'] = self._io.pos()
            self._debug['image_coordinate_rep']['start'] = self._io.pos()
            self.image_coordinate_rep = (self._io.read_bytes(1)).decode(u"UTF-8")
            self._debug['image_coordinate_rep']['end'] = self._io.pos()
            self._debug['image_geo_loc']['start'] = self._io.pos()
            self.image_geo_loc = (self._io.read_bytes(60)).decode(u"UTF-8")
            self._debug['image_geo_loc']['end'] = self._io.pos()
            self._debug['num_img_comments']['start'] = self._io.pos()
            self.num_img_comments = (self._io.read_bytes(1)).decode(u"UTF-8")
            self._debug['num_img_comments']['end'] = self._io.pos()
            self._debug['img_comments']['start'] = self._io.pos()
            self.img_comments = [None] * (int(self.num_img_comments))
            for i in range(int(self.num_img_comments)):
                if not 'arr' in self._debug['img_comments']:
                    self._debug['img_comments']['arr'] = []
                self._debug['img_comments']['arr'].append({'start': self._io.pos()})
                _t_img_comments = self._root.ImageComment(self._io, self, self._root)
                _t_img_comments._read()
                self.img_comments[i] = _t_img_comments
                self._debug['img_comments']['arr'][i]['end'] = self._io.pos()

            self._debug['img_comments']['end'] = self._io.pos()
            self._debug['img_compression']['start'] = self._io.pos()
            self.img_compression = (self._io.read_bytes(2)).decode(u"UTF-8")
            self._debug['img_compression']['end'] = self._io.pos()
            self._debug['compression_rate_code']['start'] = self._io.pos()
            self.compression_rate_code = (self._io.read_bytes(4)).decode(u"UTF-8")
            self._debug['compression_rate_code']['end'] = self._io.pos()
            self._debug['num_bands']['start'] = self._io.pos()
            self.num_bands = (self._io.read_bytes(1)).decode(u"UTF-8")
            self._debug['num_bands']['end'] = self._io.pos()
            if int(self.num_bands) == 0:
                self._debug['num_multispectral_bands']['start'] = self._io.pos()
                self.num_multispectral_bands = (self._io.read_bytes(5)).decode(u"UTF-8")
                self._debug['num_multispectral_bands']['end'] = self._io.pos()

            self._debug['bands']['start'] = self._io.pos()
            self.bands = [None] * ((int(self.num_bands) if int(self.num_bands) != 0 else int(self.num_multispectral_bands)))
            for i in range((int(self.num_bands) if int(self.num_bands) != 0 else int(self.num_multispectral_bands))):
                if not 'arr' in self._debug['bands']:
                    self._debug['bands']['arr'] = []
                self._debug['bands']['arr'].append({'start': self._io.pos()})
                _t_bands = self._root.BandInfo(self._io, self, self._root)
                _t_bands._read()
                self.bands[i] = _t_bands
                self._debug['bands']['arr'][i]['end'] = self._io.pos()

            self._debug['bands']['end'] = self._io.pos()
            self._debug['img_sync_code']['start'] = self._io.pos()
            self.img_sync_code = (self._io.read_bytes(1)).decode(u"UTF-8")
            self._debug['img_sync_code']['end'] = self._io.pos()
            self._debug['img_mode']['start'] = self._io.pos()
            self.img_mode = (self._io.read_bytes(1)).decode(u"UTF-8")
            self._debug['img_mode']['end'] = self._io.pos()
            self._debug['num_blocks_per_row']['start'] = self._io.pos()
            self.num_blocks_per_row = (self._io.read_bytes(4)).decode(u"UTF-8")
            self._debug['num_blocks_per_row']['end'] = self._io.pos()
            self._debug['num_blocks_per_col']['start'] = self._io.pos()
            self.num_blocks_per_col = (self._io.read_bytes(4)).decode(u"UTF-8")
            self._debug['num_blocks_per_col']['end'] = self._io.pos()
            self._debug['num_pixels_per_block_horz']['start'] = self._io.pos()
            self.num_pixels_per_block_horz = (self._io.read_bytes(4)).decode(u"UTF-8")
            self._debug['num_pixels_per_block_horz']['end'] = self._io.pos()
            self._debug['num_pixels_per_block_vert']['start'] = self._io.pos()
            self.num_pixels_per_block_vert = (self._io.read_bytes(4)).decode(u"UTF-8")
            self._debug['num_pixels_per_block_vert']['end'] = self._io.pos()
            self._debug['num_pixels_per_band']['start'] = self._io.pos()
            self.num_pixels_per_band = (self._io.read_bytes(2)).decode(u"UTF-8")
            self._debug['num_pixels_per_band']['end'] = self._io.pos()
            self._debug['img_display_level']['start'] = self._io.pos()
            self.img_display_level = (self._io.read_bytes(3)).decode(u"UTF-8")
            self._debug['img_display_level']['end'] = self._io.pos()
            self._debug['attachment_level']['start'] = self._io.pos()
            self.attachment_level = (self._io.read_bytes(3)).decode(u"UTF-8")
            self._debug['attachment_level']['end'] = self._io.pos()
            self._debug['img_location']['start'] = self._io.pos()
            self.img_location = (self._io.read_bytes(10)).decode(u"UTF-8")
            self._debug['img_location']['end'] = self._io.pos()
            self._debug['img_magnification']['start'] = self._io.pos()
            self.img_magnification = (self._io.read_bytes(4)).decode(u"UTF-8")
            self._debug['img_magnification']['end'] = self._io.pos()
            self._debug['user_def_img_data_len']['start'] = self._io.pos()
            self.user_def_img_data_len = (self._io.read_bytes(5)).decode(u"UTF-8")
            self._debug['user_def_img_data_len']['end'] = self._io.pos()
            if int(self.user_def_img_data_len) != 0:
                self._debug['user_def_overflow']['start'] = self._io.pos()
                self.user_def_overflow = (self._io.read_bytes(3)).decode(u"UTF-8")
                self._debug['user_def_overflow']['end'] = self._io.pos()

            if int(self.user_def_img_data_len) > 2:
                self._debug['user_def_img_data']['start'] = self._io.pos()
                self.user_def_img_data = [None] * ((int(self.user_def_img_data_len) - 3))
                for i in range((int(self.user_def_img_data_len) - 3)):
                    if not 'arr' in self._debug['user_def_img_data']:
                        self._debug['user_def_img_data']['arr'] = []
                    self._debug['user_def_img_data']['arr'].append({'start': self._io.pos()})
                    self.user_def_img_data[i] = self._io.read_u1()
                    self._debug['user_def_img_data']['arr'][i]['end'] = self._io.pos()

                self._debug['user_def_img_data']['end'] = self._io.pos()

            self._debug['image_extended_sub_header']['start'] = self._io.pos()
            self.image_extended_sub_header = self._root.TreHeader(self._io, self, self._root)
            self.image_extended_sub_header._read()
            self._debug['image_extended_sub_header']['end'] = self._io.pos()


    class ReservedSubHeader(KaitaiStruct):
        SEQ_FIELDS = ["file_part_type_re", "res_type_id", "res_version", "reclasnfo", "res_user_defined_subheader_length", "res_user_defined_subheader_fields", "res_user_defined_data"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['file_part_type_re']['start'] = self._io.pos()
            self.file_part_type_re = self._io.read_bytes(2)
            self._debug['file_part_type_re']['end'] = self._io.pos()
            if not self.file_part_type_re == b"\x52\x45":
                raise kaitaistruct.ValidationNotEqualError(b"\x52\x45", self.file_part_type_re, self._io, u"/types/reserved_sub_header/seq/0")
            self._debug['res_type_id']['start'] = self._io.pos()
            self.res_type_id = (self._io.read_bytes(25)).decode(u"UTF-8")
            self._debug['res_type_id']['end'] = self._io.pos()
            self._debug['res_version']['start'] = self._io.pos()
            self.res_version = (self._io.read_bytes(2)).decode(u"UTF-8")
            self._debug['res_version']['end'] = self._io.pos()
            self._debug['reclasnfo']['start'] = self._io.pos()
            self.reclasnfo = self._root.Clasnfo(self._io, self, self._root)
            self.reclasnfo._read()
            self._debug['reclasnfo']['end'] = self._io.pos()
            self._debug['res_user_defined_subheader_length']['start'] = self._io.pos()
            self.res_user_defined_subheader_length = (self._io.read_bytes(4)).decode(u"UTF-8")
            self._debug['res_user_defined_subheader_length']['end'] = self._io.pos()
            self._debug['res_user_defined_subheader_fields']['start'] = self._io.pos()
            self.res_user_defined_subheader_fields = (self._io.read_bytes(int(self.res_user_defined_subheader_length))).decode(u"UTF-8")
            self._debug['res_user_defined_subheader_fields']['end'] = self._io.pos()
            self._debug['res_user_defined_data']['start'] = self._io.pos()
            self.res_user_defined_data = (self._io.read_bytes_full()).decode(u"UTF-8")
            self._debug['res_user_defined_data']['end'] = self._io.pos()


    class DataSubHeaderBase(KaitaiStruct):
        SEQ_FIELDS = ["file_part_type_de", "desid", "data_definition_version", "declasnfo"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['file_part_type_de']['start'] = self._io.pos()
            self.file_part_type_de = self._io.read_bytes(2)
            self._debug['file_part_type_de']['end'] = self._io.pos()
            if not self.file_part_type_de == b"\x44\x45":
                raise kaitaistruct.ValidationNotEqualError(b"\x44\x45", self.file_part_type_de, self._io, u"/types/data_sub_header_base/seq/0")
            self._debug['desid']['start'] = self._io.pos()
            self.desid = (self._io.read_bytes(25)).decode(u"UTF-8")
            self._debug['desid']['end'] = self._io.pos()
            self._debug['data_definition_version']['start'] = self._io.pos()
            self.data_definition_version = (self._io.read_bytes(2)).decode(u"UTF-8")
            self._debug['data_definition_version']['end'] = self._io.pos()
            self._debug['declasnfo']['start'] = self._io.pos()
            self.declasnfo = self._root.Clasnfo(self._io, self, self._root)
            self.declasnfo._read()
            self._debug['declasnfo']['end'] = self._io.pos()


    class TextSubHeader(KaitaiStruct):
        SEQ_FIELDS = ["text_date_time", "text_title", "text_security_class", "encryp", "text_format", "text_extended_sub_header"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['text_date_time']['start'] = self._io.pos()
            self.text_date_time = (self._io.read_bytes(14)).decode(u"UTF-8")
            self._debug['text_date_time']['end'] = self._io.pos()
            self._debug['text_title']['start'] = self._io.pos()
            self.text_title = (self._io.read_bytes(80)).decode(u"UTF-8")
            self._debug['text_title']['end'] = self._io.pos()
            self._debug['text_security_class']['start'] = self._io.pos()
            self.text_security_class = self._root.Clasnfo(self._io, self, self._root)
            self.text_security_class._read()
            self._debug['text_security_class']['end'] = self._io.pos()
            self._debug['encryp']['start'] = self._io.pos()
            self.encryp = self._root.Encrypt(self._io, self, self._root)
            self.encryp._read()
            self._debug['encryp']['end'] = self._io.pos()
            self._debug['text_format']['start'] = self._io.pos()
            self.text_format = (self._io.read_bytes(3)).decode(u"UTF-8")
            self._debug['text_format']['end'] = self._io.pos()
            self._debug['text_extended_sub_header']['start'] = self._io.pos()
            self.text_extended_sub_header = self._root.TreHeader(self._io, self, self._root)
            self.text_extended_sub_header._read()
            self._debug['text_extended_sub_header']['end'] = self._io.pos()


    class DateTime(KaitaiStruct):
        SEQ_FIELDS = ["_unnamed0"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['_unnamed0']['start'] = self._io.pos()
            self._unnamed0 = (self._io.read_bytes(14)).decode(u"UTF-8")
            self._debug['_unnamed0']['end'] = self._io.pos()


    class Header(KaitaiStruct):
        SEQ_FIELDS = ["file_profile_name", "file_version", "complexity_level", "standard_type", "originating_station_id", "file_date_time", "file_title", "file_security", "file_copy_number", "file_num_of_copys", "encryption", "file_bg_color", "originator_name", "originator_phone", "file_length", "file_header_length", "num_image_segments", "linfo", "num_graphics_segments", "lnnfo", "reserved_numx", "num_text_files", "ltnfo", "num_data_extension", "ldnfo", "num_reserved_extension", "lrnfo", "user_defined_header", "extended_header"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['file_profile_name']['start'] = self._io.pos()
            self.file_profile_name = self._io.read_bytes(4)
            self._debug['file_profile_name']['end'] = self._io.pos()
            if not self.file_profile_name == b"\x4E\x49\x54\x46":
                raise kaitaistruct.ValidationNotEqualError(b"\x4E\x49\x54\x46", self.file_profile_name, self._io, u"/types/header/seq/0")
            self._debug['file_version']['start'] = self._io.pos()
            self.file_version = self._io.read_bytes(5)
            self._debug['file_version']['end'] = self._io.pos()
            if not self.file_version == b"\x30\x32\x2E\x31\x30":
                raise kaitaistruct.ValidationNotEqualError(b"\x30\x32\x2E\x31\x30", self.file_version, self._io, u"/types/header/seq/1")
            self._debug['complexity_level']['start'] = self._io.pos()
            self.complexity_level = self._io.read_bytes(2)
            self._debug['complexity_level']['end'] = self._io.pos()
            self._debug['standard_type']['start'] = self._io.pos()
            self.standard_type = self._io.read_bytes(4)
            self._debug['standard_type']['end'] = self._io.pos()
            if not self.standard_type == b"\x42\x46\x30\x31":
                raise kaitaistruct.ValidationNotEqualError(b"\x42\x46\x30\x31", self.standard_type, self._io, u"/types/header/seq/3")
            self._debug['originating_station_id']['start'] = self._io.pos()
            self.originating_station_id = (self._io.read_bytes(10)).decode(u"UTF-8")
            self._debug['originating_station_id']['end'] = self._io.pos()
            self._debug['file_date_time']['start'] = self._io.pos()
            self.file_date_time = self._root.DateTime(self._io, self, self._root)
            self.file_date_time._read()
            self._debug['file_date_time']['end'] = self._io.pos()
            self._debug['file_title']['start'] = self._io.pos()
            self.file_title = (self._io.read_bytes(80)).decode(u"UTF-8")
            self._debug['file_title']['end'] = self._io.pos()
            self._debug['file_security']['start'] = self._io.pos()
            self.file_security = self._root.Clasnfo(self._io, self, self._root)
            self.file_security._read()
            self._debug['file_security']['end'] = self._io.pos()
            self._debug['file_copy_number']['start'] = self._io.pos()
            self.file_copy_number = (self._io.read_bytes(5)).decode(u"UTF-8")
            self._debug['file_copy_number']['end'] = self._io.pos()
            self._debug['file_num_of_copys']['start'] = self._io.pos()
            self.file_num_of_copys = (self._io.read_bytes(5)).decode(u"UTF-8")
            self._debug['file_num_of_copys']['end'] = self._io.pos()
            self._debug['encryption']['start'] = self._io.pos()
            self.encryption = self._root.Encrypt(self._io, self, self._root)
            self.encryption._read()
            self._debug['encryption']['end'] = self._io.pos()
            self._debug['file_bg_color']['start'] = self._io.pos()
            self.file_bg_color = self._io.read_bytes(3)
            self._debug['file_bg_color']['end'] = self._io.pos()
            self._debug['originator_name']['start'] = self._io.pos()
            self.originator_name = (self._io.read_bytes(24)).decode(u"UTF-8")
            self._debug['originator_name']['end'] = self._io.pos()
            self._debug['originator_phone']['start'] = self._io.pos()
            self.originator_phone = (self._io.read_bytes(18)).decode(u"UTF-8")
            self._debug['originator_phone']['end'] = self._io.pos()
            self._debug['file_length']['start'] = self._io.pos()
            self.file_length = (self._io.read_bytes(12)).decode(u"UTF-8")
            self._debug['file_length']['end'] = self._io.pos()
            self._debug['file_header_length']['start'] = self._io.pos()
            self.file_header_length = (self._io.read_bytes(6)).decode(u"UTF-8")
            self._debug['file_header_length']['end'] = self._io.pos()
            self._debug['num_image_segments']['start'] = self._io.pos()
            self.num_image_segments = (self._io.read_bytes(3)).decode(u"UTF-8")
            self._debug['num_image_segments']['end'] = self._io.pos()
            self._debug['linfo']['start'] = self._io.pos()
            self.linfo = [None] * (int(self.num_image_segments))
            for i in range(int(self.num_image_segments)):
                if not 'arr' in self._debug['linfo']:
                    self._debug['linfo']['arr'] = []
                self._debug['linfo']['arr'].append({'start': self._io.pos()})
                _t_linfo = self._root.LengthImageInfo(self._io, self, self._root)
                _t_linfo._read()
                self.linfo[i] = _t_linfo
                self._debug['linfo']['arr'][i]['end'] = self._io.pos()

            self._debug['linfo']['end'] = self._io.pos()
            self._debug['num_graphics_segments']['start'] = self._io.pos()
            self.num_graphics_segments = (self._io.read_bytes(3)).decode(u"UTF-8")
            self._debug['num_graphics_segments']['end'] = self._io.pos()
            self._debug['lnnfo']['start'] = self._io.pos()
            self.lnnfo = [None] * (int(self.num_graphics_segments))
            for i in range(int(self.num_graphics_segments)):
                if not 'arr' in self._debug['lnnfo']:
                    self._debug['lnnfo']['arr'] = []
                self._debug['lnnfo']['arr'].append({'start': self._io.pos()})
                _t_lnnfo = self._root.LengthGraphicInfo(self._io, self, self._root)
                _t_lnnfo._read()
                self.lnnfo[i] = _t_lnnfo
                self._debug['lnnfo']['arr'][i]['end'] = self._io.pos()

            self._debug['lnnfo']['end'] = self._io.pos()
            self._debug['reserved_numx']['start'] = self._io.pos()
            self.reserved_numx = (self._io.read_bytes(3)).decode(u"UTF-8")
            self._debug['reserved_numx']['end'] = self._io.pos()
            self._debug['num_text_files']['start'] = self._io.pos()
            self.num_text_files = (self._io.read_bytes(3)).decode(u"UTF-8")
            self._debug['num_text_files']['end'] = self._io.pos()
            self._debug['ltnfo']['start'] = self._io.pos()
            self.ltnfo = [None] * (int(self.num_text_files))
            for i in range(int(self.num_text_files)):
                if not 'arr' in self._debug['ltnfo']:
                    self._debug['ltnfo']['arr'] = []
                self._debug['ltnfo']['arr'].append({'start': self._io.pos()})
                _t_ltnfo = self._root.LengthTextInfo(self._io, self, self._root)
                _t_ltnfo._read()
                self.ltnfo[i] = _t_ltnfo
                self._debug['ltnfo']['arr'][i]['end'] = self._io.pos()

            self._debug['ltnfo']['end'] = self._io.pos()
            self._debug['num_data_extension']['start'] = self._io.pos()
            self.num_data_extension = (self._io.read_bytes(3)).decode(u"UTF-8")
            self._debug['num_data_extension']['end'] = self._io.pos()
            self._debug['ldnfo']['start'] = self._io.pos()
            self.ldnfo = [None] * (int(self.num_data_extension))
            for i in range(int(self.num_data_extension)):
                if not 'arr' in self._debug['ldnfo']:
                    self._debug['ldnfo']['arr'] = []
                self._debug['ldnfo']['arr'].append({'start': self._io.pos()})
                _t_ldnfo = self._root.LengthDataInfo(self._io, self, self._root)
                _t_ldnfo._read()
                self.ldnfo[i] = _t_ldnfo
                self._debug['ldnfo']['arr'][i]['end'] = self._io.pos()

            self._debug['ldnfo']['end'] = self._io.pos()
            self._debug['num_reserved_extension']['start'] = self._io.pos()
            self.num_reserved_extension = (self._io.read_bytes(3)).decode(u"UTF-8")
            self._debug['num_reserved_extension']['end'] = self._io.pos()
            self._debug['lrnfo']['start'] = self._io.pos()
            self.lrnfo = [None] * (int(self.num_reserved_extension))
            for i in range(int(self.num_reserved_extension)):
                if not 'arr' in self._debug['lrnfo']:
                    self._debug['lrnfo']['arr'] = []
                self._debug['lrnfo']['arr'].append({'start': self._io.pos()})
                _t_lrnfo = self._root.LengthReservedInfo(self._io, self, self._root)
                _t_lrnfo._read()
                self.lrnfo[i] = _t_lrnfo
                self._debug['lrnfo']['arr'][i]['end'] = self._io.pos()

            self._debug['lrnfo']['end'] = self._io.pos()
            self._debug['user_defined_header']['start'] = self._io.pos()
            self.user_defined_header = self._root.TreHeader(self._io, self, self._root)
            self.user_defined_header._read()
            self._debug['user_defined_header']['end'] = self._io.pos()
            self._debug['extended_header']['start'] = self._io.pos()
            self.extended_header = self._root.TreHeader(self._io, self, self._root)
            self.extended_header._read()
            self._debug['extended_header']['end'] = self._io.pos()


    class DataSubHeaderStreaming(KaitaiStruct):
        """Streaming file Header Data Extension Segment Subheader."""
        SEQ_FIELDS = ["des_base", "des_defined_subheader_fields_len", "sfh_l1", "sfh_delim1", "sfh_dr", "sfh_delim2", "sfh_l2"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['des_base']['start'] = self._io.pos()
            self.des_base = self._root.DataSubHeaderBase(self._io, self, self._root)
            self.des_base._read()
            self._debug['des_base']['end'] = self._io.pos()
            self._debug['des_defined_subheader_fields_len']['start'] = self._io.pos()
            self.des_defined_subheader_fields_len = (self._io.read_bytes(4)).decode(u"UTF-8")
            self._debug['des_defined_subheader_fields_len']['end'] = self._io.pos()
            self._debug['sfh_l1']['start'] = self._io.pos()
            self.sfh_l1 = (self._io.read_bytes(7)).decode(u"UTF-8")
            self._debug['sfh_l1']['end'] = self._io.pos()
            self._debug['sfh_delim1']['start'] = self._io.pos()
            self.sfh_delim1 = self._io.read_u4be()
            self._debug['sfh_delim1']['end'] = self._io.pos()
            self._debug['sfh_dr']['start'] = self._io.pos()
            self.sfh_dr = [None] * (int(self.sfh_l1))
            for i in range(int(self.sfh_l1)):
                if not 'arr' in self._debug['sfh_dr']:
                    self._debug['sfh_dr']['arr'] = []
                self._debug['sfh_dr']['arr'].append({'start': self._io.pos()})
                self.sfh_dr[i] = self._io.read_u1()
                self._debug['sfh_dr']['arr'][i]['end'] = self._io.pos()

            self._debug['sfh_dr']['end'] = self._io.pos()
            self._debug['sfh_delim2']['start'] = self._io.pos()
            self.sfh_delim2 = self._io.read_u4be()
            self._debug['sfh_delim2']['end'] = self._io.pos()
            self._debug['sfh_l2']['start'] = self._io.pos()
            self.sfh_l2 = (self._io.read_bytes(7)).decode(u"UTF-8")
            self._debug['sfh_l2']['end'] = self._io.pos()


    class TreHeader(KaitaiStruct):
        SEQ_FIELDS = ["header_data_length", "header_overflow", "header_data"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['header_data_length']['start'] = self._io.pos()
            self.header_data_length = (self._io.read_bytes(5)).decode(u"UTF-8")
            self._debug['header_data_length']['end'] = self._io.pos()
            if int(self.header_data_length) != 0:
                self._debug['header_overflow']['start'] = self._io.pos()
                self.header_overflow = (self._io.read_bytes(3)).decode(u"UTF-8")
                self._debug['header_overflow']['end'] = self._io.pos()

            if int(self.header_data_length) > 2:
                self._debug['header_data']['start'] = self._io.pos()
                self.header_data = [None] * ((int(self.header_data_length) - 3))
                for i in range((int(self.header_data_length) - 3)):
                    if not 'arr' in self._debug['header_data']:
                        self._debug['header_data']['arr'] = []
                    self._debug['header_data']['arr'].append({'start': self._io.pos()})
                    self.header_data[i] = self._io.read_u1()
                    self._debug['header_data']['arr'][i]['end'] = self._io.pos()

                self._debug['header_data']['end'] = self._io.pos()



    class LengthImageInfo(KaitaiStruct):
        SEQ_FIELDS = ["length_image_subheader", "length_image_segment"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['length_image_subheader']['start'] = self._io.pos()
            self.length_image_subheader = (self._io.read_bytes(6)).decode(u"UTF-8")
            self._debug['length_image_subheader']['end'] = self._io.pos()
            self._debug['length_image_segment']['start'] = self._io.pos()
            self.length_image_segment = (self._io.read_bytes(10)).decode(u"UTF-8")
            self._debug['length_image_segment']['end'] = self._io.pos()


    class LengthDataInfo(KaitaiStruct):
        SEQ_FIELDS = ["length_data_extension_subheader", "length_data_extension_segment"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['length_data_extension_subheader']['start'] = self._io.pos()
            self.length_data_extension_subheader = (self._io.read_bytes(4)).decode(u"UTF-8")
            self._debug['length_data_extension_subheader']['end'] = self._io.pos()
            self._debug['length_data_extension_segment']['start'] = self._io.pos()
            self.length_data_extension_segment = (self._io.read_bytes(9)).decode(u"UTF-8")
            self._debug['length_data_extension_segment']['end'] = self._io.pos()


    class LengthTextInfo(KaitaiStruct):
        SEQ_FIELDS = ["length_text_subheader", "length_text_segment"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['length_text_subheader']['start'] = self._io.pos()
            self.length_text_subheader = (self._io.read_bytes(4)).decode(u"UTF-8")
            self._debug['length_text_subheader']['end'] = self._io.pos()
            self._debug['length_text_segment']['start'] = self._io.pos()
            self.length_text_segment = (self._io.read_bytes(5)).decode(u"UTF-8")
            self._debug['length_text_segment']['end'] = self._io.pos()



