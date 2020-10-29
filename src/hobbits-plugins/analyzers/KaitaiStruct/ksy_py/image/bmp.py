# This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

from pkg_resources import parse_version
import kaitaistruct
from kaitaistruct import KaitaiStruct, KaitaiStream, BytesIO
from enum import Enum
import collections


if parse_version(kaitaistruct.__version__) < parse_version('0.9'):
    raise Exception("Incompatible Kaitai Struct Python API: 0.9 or later is required, but you have %s" % (kaitaistruct.__version__))

class Bmp(KaitaiStruct):
    """The **BMP file format**, also known as **bitmap image file** or **device independent
    bitmap (DIB) file format** or simply a **bitmap**, is a raster graphics image file
    format used to store bitmap digital images, independently of the display
    device (such as a graphics adapter), especially on Microsoft Windows
    and OS/2 operating systems.
    
    ## Samples
    
    Great collection of various BMP sample files:
    [**BMP Suite Image List**](
      http://entropymine.com/jason/bmpsuite/bmpsuite/html/bmpsuite.html
    ) (by Jason Summers)
    
    If only there was such a comprehensive sample suite for every file format! It's like
    a dream for every developer of any binary file format parser. It contains a lot of
    different types and variations of BMP files, even the tricky ones, where it's not clear
    from the specification how to deal with them (marked there as "**q**uestionable").
    
    If you make a program which will be able to read all the "**g**ood" and "**q**uestionable"
    BMP files and won't crash on the "**b**ad" ones, it will definitely have one of the most
    extensive support of BMP files in the universe!
    
    ## BITMAPV2INFOHEADER and BITMAPV3INFOHEADER
    
    A beneficial discussion on Adobe forum (archived):
    [**Invalid BMP Format with Alpha channel**](
      https://web.archive.org/web/20150127132443/https://forums.adobe.com/message/3272950
    )
    
    In 2010, someone noticed that Photoshop generated BMP with an odd type of header. There wasn't
    any documentation available for this header at the time (and still isn't).
    However, Chris Cox (former Adobe employee) claimed that they hadn't invented any type
    of proprietary header and everything they were writing was taken directly
    from the Microsoft documentation.
    
    It showed up that the unknown header was called BITMAPV3INFOHEADER.
    Although Microsoft has apparently requested and verified the use of the header,
    the documentation on MSDN has probably got lost and they have probably
    forgotten about this type of header.
    
    This is the only source I could find about these structures, so we could't rely
    on it so much, but I think supporting them as a read-only format won't harm anything.
    Due to the fact that it isn't documented anywhere else, most applications don't support it.
    
    All Windows headers at once (including mentioned BITMAPV2INFOHEADER and BITMAPV3INFOHEADER):
    
    ![Bitmap headers overview](
      https://web.archive.org/web/20190527043845/https://forums.adobe.com/servlet/JiveServlet/showImage/2-3273299-47801/BMP_Headers.png
    )
    
    ## Specs
     * [Bitmap Storage (Windows Dev Center)](
         https://docs.microsoft.com/en-us/windows/win32/gdi/bitmap-storage
       )
        * BITMAPFILEHEADER
        * BITMAPINFOHEADER
        * BITMAPV4HEADER
        * BITMAPV5HEADER
     * [OS/2 Bitmap File Format](
          https://www.fileformat.info/format/os2bmp/egff.htm
       )
        * BITMAPFILEHEADER (OS2BMPFILEHEADER)
        * BITMAPCOREHEADER (OS21XBITMAPHEADER)
        * OS22XBITMAPHEADER
     * [Microsoft Windows Bitmap](
          http://netghost.narod.ru/gff/graphics/summary/micbmp.htm
       )
        * BITMAPFILEHEADER (WINBMPFILEHEADER)
        * BITMAPCOREHEADER (WIN2XBITMAPHEADER)
        * BITMAPINFOHEADER (WINNTBITMAPHEADER)
        * BITMAPV4HEADER (WIN4XBITMAPHEADER)
    """

    class Intent(Enum):
        business = 1
        graphics = 2
        images = 4
        abs_colorimetric = 8

    class ColorSpace(Enum):
        calibrated_rgb = 0
        profile_linked = 1279872587
        profile_embedded = 1296188740
        windows = 1466527264
        s_rgb = 1934772034

    class Os2Rendering(Enum):
        no_halftoning = 0
        error_diffusion = 1
        panda = 2
        super_circle = 3

    class HeaderType(Enum):
        bitmap_core_header = 12
        bitmap_info_header = 40
        bitmap_v2_info_header = 52
        bitmap_v3_info_header = 56
        os2_2x_bitmap_header = 64
        bitmap_v4_header = 108
        bitmap_v5_header = 124

    class Compressions(Enum):
        rgb = 0
        rle8 = 1
        rle4 = 2
        bitfields = 3
        jpeg = 4
        png = 5
        alpha_bitfields = 6

    class Os2Compressions(Enum):
        rgb = 0
        rle8 = 1
        rle4 = 2
        huffman_1d = 3
        rle24 = 4
    SEQ_FIELDS = ["file_hdr", "dib_info", "bitmap"]
    def __init__(self, _io, _parent=None, _root=None):
        self._io = _io
        self._parent = _parent
        self._root = _root if _root else self
        self._debug = collections.defaultdict(dict)

    def _read(self):
        self._debug['file_hdr']['start'] = self._io.pos()
        self.file_hdr = Bmp.FileHeader(self._io, self, self._root)
        self.file_hdr._read()
        self._debug['file_hdr']['end'] = self._io.pos()
        self._debug['dib_info']['start'] = self._io.pos()
        self._raw_dib_info = self._io.read_bytes((self.file_hdr.ofs_bitmap - 14))
        _io__raw_dib_info = KaitaiStream(BytesIO(self._raw_dib_info))
        self.dib_info = Bmp.BitmapInfo(_io__raw_dib_info, self, self._root)
        self.dib_info._read()
        self._debug['dib_info']['end'] = self._io.pos()
        self._debug['bitmap']['start'] = self._io.pos()
        self._raw_bitmap = self._io.read_bytes_full()
        _io__raw_bitmap = KaitaiStream(BytesIO(self._raw_bitmap))
        self.bitmap = Bmp.Bitmap(_io__raw_bitmap, self, self._root)
        self.bitmap._read()
        self._debug['bitmap']['end'] = self._io.pos()

    class CieXyz(KaitaiStruct):
        """
        .. seealso::
           Source - https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-ciexyz
        """
        SEQ_FIELDS = ["x", "y", "z"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['x']['start'] = self._io.pos()
            self.x = Bmp.FixedPoint2Dot30(self._io, self, self._root)
            self.x._read()
            self._debug['x']['end'] = self._io.pos()
            self._debug['y']['start'] = self._io.pos()
            self.y = Bmp.FixedPoint2Dot30(self._io, self, self._root)
            self.y._read()
            self._debug['y']['end'] = self._io.pos()
            self._debug['z']['start'] = self._io.pos()
            self.z = Bmp.FixedPoint2Dot30(self._io, self, self._root)
            self.z._read()
            self._debug['z']['end'] = self._io.pos()


    class RgbRecord(KaitaiStruct):
        SEQ_FIELDS = ["blue", "green", "red", "reserved"]
        def __init__(self, has_reserved_field, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self.has_reserved_field = has_reserved_field
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['blue']['start'] = self._io.pos()
            self.blue = self._io.read_u1()
            self._debug['blue']['end'] = self._io.pos()
            self._debug['green']['start'] = self._io.pos()
            self.green = self._io.read_u1()
            self._debug['green']['end'] = self._io.pos()
            self._debug['red']['start'] = self._io.pos()
            self.red = self._io.read_u1()
            self._debug['red']['end'] = self._io.pos()
            if self.has_reserved_field:
                self._debug['reserved']['start'] = self._io.pos()
                self.reserved = self._io.read_u1()
                self._debug['reserved']['end'] = self._io.pos()



    class BitmapV5Extension(KaitaiStruct):
        """
        .. seealso::
           Source - https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapv5header
        """
        SEQ_FIELDS = ["intent", "ofs_profile", "len_profile", "reserved"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['intent']['start'] = self._io.pos()
            self.intent = KaitaiStream.resolve_enum(Bmp.Intent, self._io.read_u4le())
            self._debug['intent']['end'] = self._io.pos()
            self._debug['ofs_profile']['start'] = self._io.pos()
            self.ofs_profile = self._io.read_u4le()
            self._debug['ofs_profile']['end'] = self._io.pos()
            self._debug['len_profile']['start'] = self._io.pos()
            self.len_profile = self._io.read_u4le()
            self._debug['len_profile']['end'] = self._io.pos()
            self._debug['reserved']['start'] = self._io.pos()
            self.reserved = self._io.read_u4le()
            self._debug['reserved']['end'] = self._io.pos()

        @property
        def has_profile(self):
            if hasattr(self, '_m_has_profile'):
                return self._m_has_profile if hasattr(self, '_m_has_profile') else None

            self._m_has_profile =  ((self._parent.bitmap_v4_ext.color_space_type == Bmp.ColorSpace.profile_linked) or (self._parent.bitmap_v4_ext.color_space_type == Bmp.ColorSpace.profile_embedded)) 
            return self._m_has_profile if hasattr(self, '_m_has_profile') else None

        @property
        def profile_data(self):
            """
            .. seealso::
               "If the profile is embedded, profile data is the actual profile, and if it is linked, the profile data is the null-terminated file name of the profile. This cannot be a Unicode string. It must be composed exclusively of characters from the Windows character set (code page 1252)." - https://docs.microsoft.com/en-us/previous-versions/windows/desktop/wcs/using-structures-in-wcs-1-0
            """
            if hasattr(self, '_m_profile_data'):
                return self._m_profile_data if hasattr(self, '_m_profile_data') else None

            if self.has_profile:
                io = self._root._io
                _pos = io.pos()
                io.seek((14 + self.ofs_profile))
                self._debug['_m_profile_data']['start'] = io.pos()
                _on = self._parent.bitmap_v4_ext.color_space_type == Bmp.ColorSpace.profile_linked
                if _on == True:
                    self._m_profile_data = (KaitaiStream.bytes_terminate(io.read_bytes(self.len_profile), 0, False)).decode(u"windows-1252")
                else:
                    self._m_profile_data = io.read_bytes(self.len_profile)
                self._debug['_m_profile_data']['end'] = io.pos()
                io.seek(_pos)

            return self._m_profile_data if hasattr(self, '_m_profile_data') else None


    class ColorMask(KaitaiStruct):
        SEQ_FIELDS = ["red_mask", "green_mask", "blue_mask", "alpha_mask"]
        def __init__(self, has_alpha_mask, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self.has_alpha_mask = has_alpha_mask
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['red_mask']['start'] = self._io.pos()
            self.red_mask = self._io.read_u4le()
            self._debug['red_mask']['end'] = self._io.pos()
            self._debug['green_mask']['start'] = self._io.pos()
            self.green_mask = self._io.read_u4le()
            self._debug['green_mask']['end'] = self._io.pos()
            self._debug['blue_mask']['start'] = self._io.pos()
            self.blue_mask = self._io.read_u4le()
            self._debug['blue_mask']['end'] = self._io.pos()
            if self.has_alpha_mask:
                self._debug['alpha_mask']['start'] = self._io.pos()
                self.alpha_mask = self._io.read_u4le()
                self._debug['alpha_mask']['end'] = self._io.pos()



    class BitmapV4Extension(KaitaiStruct):
        """
        .. seealso::
           Source - https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapv4header
        """
        SEQ_FIELDS = ["color_space_type", "endpoint_red", "endpoint_green", "endpoint_blue", "gamma_red", "gamma_blue", "gamma_green"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['color_space_type']['start'] = self._io.pos()
            self.color_space_type = KaitaiStream.resolve_enum(Bmp.ColorSpace, self._io.read_u4le())
            self._debug['color_space_type']['end'] = self._io.pos()
            self._debug['endpoint_red']['start'] = self._io.pos()
            self.endpoint_red = Bmp.CieXyz(self._io, self, self._root)
            self.endpoint_red._read()
            self._debug['endpoint_red']['end'] = self._io.pos()
            self._debug['endpoint_green']['start'] = self._io.pos()
            self.endpoint_green = Bmp.CieXyz(self._io, self, self._root)
            self.endpoint_green._read()
            self._debug['endpoint_green']['end'] = self._io.pos()
            self._debug['endpoint_blue']['start'] = self._io.pos()
            self.endpoint_blue = Bmp.CieXyz(self._io, self, self._root)
            self.endpoint_blue._read()
            self._debug['endpoint_blue']['end'] = self._io.pos()
            self._debug['gamma_red']['start'] = self._io.pos()
            self.gamma_red = Bmp.FixedPoint16Dot16(self._io, self, self._root)
            self.gamma_red._read()
            self._debug['gamma_red']['end'] = self._io.pos()
            self._debug['gamma_blue']['start'] = self._io.pos()
            self.gamma_blue = Bmp.FixedPoint16Dot16(self._io, self, self._root)
            self.gamma_blue._read()
            self._debug['gamma_blue']['end'] = self._io.pos()
            self._debug['gamma_green']['start'] = self._io.pos()
            self.gamma_green = Bmp.FixedPoint16Dot16(self._io, self, self._root)
            self.gamma_green._read()
            self._debug['gamma_green']['end'] = self._io.pos()


    class BitmapInfoExtension(KaitaiStruct):
        """
        .. seealso::
           Source - https://docs.microsoft.com/en-us/previous-versions/dd183376(v=vs.85)
        """
        SEQ_FIELDS = ["compression", "os2_compression", "len_image", "x_resolution", "y_resolution", "num_colors_used", "num_colors_important"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            if not (self._parent.extends_os2_2x_bitmap):
                self._debug['compression']['start'] = self._io.pos()
                self.compression = KaitaiStream.resolve_enum(Bmp.Compressions, self._io.read_u4le())
                self._debug['compression']['end'] = self._io.pos()

            if self._parent.extends_os2_2x_bitmap:
                self._debug['os2_compression']['start'] = self._io.pos()
                self.os2_compression = KaitaiStream.resolve_enum(Bmp.Os2Compressions, self._io.read_u4le())
                self._debug['os2_compression']['end'] = self._io.pos()

            self._debug['len_image']['start'] = self._io.pos()
            self.len_image = self._io.read_u4le()
            self._debug['len_image']['end'] = self._io.pos()
            self._debug['x_resolution']['start'] = self._io.pos()
            self.x_resolution = self._io.read_u4le()
            self._debug['x_resolution']['end'] = self._io.pos()
            self._debug['y_resolution']['start'] = self._io.pos()
            self.y_resolution = self._io.read_u4le()
            self._debug['y_resolution']['end'] = self._io.pos()
            self._debug['num_colors_used']['start'] = self._io.pos()
            self.num_colors_used = self._io.read_u4le()
            self._debug['num_colors_used']['end'] = self._io.pos()
            self._debug['num_colors_important']['start'] = self._io.pos()
            self.num_colors_important = self._io.read_u4le()
            self._debug['num_colors_important']['end'] = self._io.pos()


    class FixedPoint2Dot30(KaitaiStruct):
        SEQ_FIELDS = ["raw"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['raw']['start'] = self._io.pos()
            self.raw = self._io.read_u4le()
            self._debug['raw']['end'] = self._io.pos()

        @property
        def value(self):
            if hasattr(self, '_m_value'):
                return self._m_value if hasattr(self, '_m_value') else None

            self._m_value = (self.raw / (1 << 30))
            return self._m_value if hasattr(self, '_m_value') else None


    class Bitmap(KaitaiStruct):
        """Replace with an opaque type if you care about the pixels.
        You can look at an example of a JavaScript implementation: https://github.com/generalmimon/bmptool/blob/master/src/Bitmap.js
        
        There is a proposal for adding bitmap data type to Kaitai Struct: https://github.com/kaitai-io/kaitai_struct/issues/188
        """
        SEQ_FIELDS = []
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            pass


    class BitmapHeader(KaitaiStruct):
        """
        .. seealso::
           Source - https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapcoreheader
        
        
        .. seealso::
           Source - https://www.fileformat.info/format/os2bmp/egff.htm#OS2BMP-DMYID.3.1
        """
        SEQ_FIELDS = ["image_width", "image_height_raw", "num_planes", "bits_per_pixel", "bitmap_info_ext", "color_mask", "os2_2x_bitmap_ext", "bitmap_v4_ext", "bitmap_v5_ext"]
        def __init__(self, len_header, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self.len_header = len_header
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['image_width']['start'] = self._io.pos()
            _on = self.is_core_header
            if _on == True:
                self.image_width = self._io.read_u2le()
            elif _on == False:
                self.image_width = self._io.read_u4le()
            self._debug['image_width']['end'] = self._io.pos()
            self._debug['image_height_raw']['start'] = self._io.pos()
            _on = self.is_core_header
            if _on == True:
                self.image_height_raw = self._io.read_s2le()
            elif _on == False:
                self.image_height_raw = self._io.read_s4le()
            self._debug['image_height_raw']['end'] = self._io.pos()
            self._debug['num_planes']['start'] = self._io.pos()
            self.num_planes = self._io.read_u2le()
            self._debug['num_planes']['end'] = self._io.pos()
            self._debug['bits_per_pixel']['start'] = self._io.pos()
            self.bits_per_pixel = self._io.read_u2le()
            self._debug['bits_per_pixel']['end'] = self._io.pos()
            if self.extends_bitmap_info:
                self._debug['bitmap_info_ext']['start'] = self._io.pos()
                self.bitmap_info_ext = Bmp.BitmapInfoExtension(self._io, self, self._root)
                self.bitmap_info_ext._read()
                self._debug['bitmap_info_ext']['end'] = self._io.pos()

            if self.is_color_mask_here:
                self._debug['color_mask']['start'] = self._io.pos()
                self.color_mask = Bmp.ColorMask(self.len_header != Bmp.HeaderType.bitmap_v2_info_header.value, self._io, self, self._root)
                self.color_mask._read()
                self._debug['color_mask']['end'] = self._io.pos()

            if self.extends_os2_2x_bitmap:
                self._debug['os2_2x_bitmap_ext']['start'] = self._io.pos()
                self.os2_2x_bitmap_ext = Bmp.Os22xBitmapExtension(self._io, self, self._root)
                self.os2_2x_bitmap_ext._read()
                self._debug['os2_2x_bitmap_ext']['end'] = self._io.pos()

            if self.extends_bitmap_v4:
                self._debug['bitmap_v4_ext']['start'] = self._io.pos()
                self.bitmap_v4_ext = Bmp.BitmapV4Extension(self._io, self, self._root)
                self.bitmap_v4_ext._read()
                self._debug['bitmap_v4_ext']['end'] = self._io.pos()

            if self.extends_bitmap_v5:
                self._debug['bitmap_v5_ext']['start'] = self._io.pos()
                self.bitmap_v5_ext = Bmp.BitmapV5Extension(self._io, self, self._root)
                self.bitmap_v5_ext._read()
                self._debug['bitmap_v5_ext']['end'] = self._io.pos()


        @property
        def extends_bitmap_v4(self):
            if hasattr(self, '_m_extends_bitmap_v4'):
                return self._m_extends_bitmap_v4 if hasattr(self, '_m_extends_bitmap_v4') else None

            self._m_extends_bitmap_v4 = self.len_header >= Bmp.HeaderType.bitmap_v4_header.value
            return self._m_extends_bitmap_v4 if hasattr(self, '_m_extends_bitmap_v4') else None

        @property
        def extends_os2_2x_bitmap(self):
            if hasattr(self, '_m_extends_os2_2x_bitmap'):
                return self._m_extends_os2_2x_bitmap if hasattr(self, '_m_extends_os2_2x_bitmap') else None

            self._m_extends_os2_2x_bitmap = self.len_header == Bmp.HeaderType.os2_2x_bitmap_header.value
            return self._m_extends_os2_2x_bitmap if hasattr(self, '_m_extends_os2_2x_bitmap') else None

        @property
        def uses_fixed_palette(self):
            if hasattr(self, '_m_uses_fixed_palette'):
                return self._m_uses_fixed_palette if hasattr(self, '_m_uses_fixed_palette') else None

            self._m_uses_fixed_palette =  ((not ( ((self.bits_per_pixel == 16) or (self.bits_per_pixel == 24) or (self.bits_per_pixel == 32)) )) and (not ( ((self.extends_bitmap_info) and (not (self.extends_os2_2x_bitmap)) and ( ((self.bitmap_info_ext.compression == Bmp.Compressions.jpeg) or (self.bitmap_info_ext.compression == Bmp.Compressions.png)) )) ))) 
            return self._m_uses_fixed_palette if hasattr(self, '_m_uses_fixed_palette') else None

        @property
        def extends_bitmap_info(self):
            if hasattr(self, '_m_extends_bitmap_info'):
                return self._m_extends_bitmap_info if hasattr(self, '_m_extends_bitmap_info') else None

            self._m_extends_bitmap_info = self.len_header >= Bmp.HeaderType.bitmap_info_header.value
            return self._m_extends_bitmap_info if hasattr(self, '_m_extends_bitmap_info') else None

        @property
        def image_height(self):
            if hasattr(self, '_m_image_height'):
                return self._m_image_height if hasattr(self, '_m_image_height') else None

            self._m_image_height = (-(self.image_height_raw) if self.image_height_raw < 0 else self.image_height_raw)
            return self._m_image_height if hasattr(self, '_m_image_height') else None

        @property
        def is_core_header(self):
            if hasattr(self, '_m_is_core_header'):
                return self._m_is_core_header if hasattr(self, '_m_is_core_header') else None

            self._m_is_core_header = self.len_header == Bmp.HeaderType.bitmap_core_header.value
            return self._m_is_core_header if hasattr(self, '_m_is_core_header') else None

        @property
        def extends_bitmap_v5(self):
            if hasattr(self, '_m_extends_bitmap_v5'):
                return self._m_extends_bitmap_v5 if hasattr(self, '_m_extends_bitmap_v5') else None

            self._m_extends_bitmap_v5 = self.len_header >= Bmp.HeaderType.bitmap_v5_header.value
            return self._m_extends_bitmap_v5 if hasattr(self, '_m_extends_bitmap_v5') else None

        @property
        def is_color_mask_here(self):
            if hasattr(self, '_m_is_color_mask_here'):
                return self._m_is_color_mask_here if hasattr(self, '_m_is_color_mask_here') else None

            self._m_is_color_mask_here =  ((self.len_header == Bmp.HeaderType.bitmap_v2_info_header.value) or (self.len_header == Bmp.HeaderType.bitmap_v3_info_header.value) or (self.extends_bitmap_v4)) 
            return self._m_is_color_mask_here if hasattr(self, '_m_is_color_mask_here') else None

        @property
        def bottom_up(self):
            if hasattr(self, '_m_bottom_up'):
                return self._m_bottom_up if hasattr(self, '_m_bottom_up') else None

            self._m_bottom_up = self.image_height_raw > 0
            return self._m_bottom_up if hasattr(self, '_m_bottom_up') else None


    class Os22xBitmapExtension(KaitaiStruct):
        """
        .. seealso::
           Source - https://www.fileformat.info/format/os2bmp/egff.htm#OS2BMP-DMYID.3.2
        """
        SEQ_FIELDS = ["units", "reserved", "recording", "rendering", "size1", "size2", "color_encoding", "identifier"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['units']['start'] = self._io.pos()
            self.units = self._io.read_u2le()
            self._debug['units']['end'] = self._io.pos()
            self._debug['reserved']['start'] = self._io.pos()
            self.reserved = self._io.read_u2le()
            self._debug['reserved']['end'] = self._io.pos()
            self._debug['recording']['start'] = self._io.pos()
            self.recording = self._io.read_u2le()
            self._debug['recording']['end'] = self._io.pos()
            self._debug['rendering']['start'] = self._io.pos()
            self.rendering = KaitaiStream.resolve_enum(Bmp.Os2Rendering, self._io.read_u2le())
            self._debug['rendering']['end'] = self._io.pos()
            self._debug['size1']['start'] = self._io.pos()
            self.size1 = self._io.read_u4le()
            self._debug['size1']['end'] = self._io.pos()
            self._debug['size2']['start'] = self._io.pos()
            self.size2 = self._io.read_u4le()
            self._debug['size2']['end'] = self._io.pos()
            self._debug['color_encoding']['start'] = self._io.pos()
            self.color_encoding = self._io.read_u4le()
            self._debug['color_encoding']['end'] = self._io.pos()
            self._debug['identifier']['start'] = self._io.pos()
            self.identifier = self._io.read_u4le()
            self._debug['identifier']['end'] = self._io.pos()


    class FixedPoint16Dot16(KaitaiStruct):
        SEQ_FIELDS = ["raw"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['raw']['start'] = self._io.pos()
            self.raw = self._io.read_u4le()
            self._debug['raw']['end'] = self._io.pos()

        @property
        def value(self):
            if hasattr(self, '_m_value'):
                return self._m_value if hasattr(self, '_m_value') else None

            self._m_value = (self.raw / (1 << 16))
            return self._m_value if hasattr(self, '_m_value') else None


    class ColorTable(KaitaiStruct):
        SEQ_FIELDS = ["colors"]
        def __init__(self, has_reserved_field, num_colors, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self.has_reserved_field = has_reserved_field
            self.num_colors = num_colors
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['colors']['start'] = self._io.pos()
            self.colors = [None] * ((self.num_colors if  ((self.num_colors > 0) and (self.num_colors < self.num_colors_present))  else self.num_colors_present))
            for i in range((self.num_colors if  ((self.num_colors > 0) and (self.num_colors < self.num_colors_present))  else self.num_colors_present)):
                if not 'arr' in self._debug['colors']:
                    self._debug['colors']['arr'] = []
                self._debug['colors']['arr'].append({'start': self._io.pos()})
                _t_colors = Bmp.RgbRecord(self.has_reserved_field, self._io, self, self._root)
                _t_colors._read()
                self.colors[i] = _t_colors
                self._debug['colors']['arr'][i]['end'] = self._io.pos()

            self._debug['colors']['end'] = self._io.pos()

        @property
        def num_colors_present(self):
            if hasattr(self, '_m_num_colors_present'):
                return self._m_num_colors_present if hasattr(self, '_m_num_colors_present') else None

            self._m_num_colors_present = self._io.size() // (4 if self.has_reserved_field else 3)
            return self._m_num_colors_present if hasattr(self, '_m_num_colors_present') else None


    class FileHeader(KaitaiStruct):
        """
        .. seealso::
           Source - https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapfileheader
        """
        SEQ_FIELDS = ["file_type", "len_file", "reserved1", "reserved2", "ofs_bitmap"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['file_type']['start'] = self._io.pos()
            self.file_type = self._io.read_bytes(2)
            self._debug['file_type']['end'] = self._io.pos()
            if not self.file_type == b"\x42\x4D":
                raise kaitaistruct.ValidationNotEqualError(b"\x42\x4D", self.file_type, self._io, u"/types/file_header/seq/0")
            self._debug['len_file']['start'] = self._io.pos()
            self.len_file = self._io.read_u4le()
            self._debug['len_file']['end'] = self._io.pos()
            self._debug['reserved1']['start'] = self._io.pos()
            self.reserved1 = self._io.read_u2le()
            self._debug['reserved1']['end'] = self._io.pos()
            self._debug['reserved2']['start'] = self._io.pos()
            self.reserved2 = self._io.read_u2le()
            self._debug['reserved2']['end'] = self._io.pos()
            self._debug['ofs_bitmap']['start'] = self._io.pos()
            self.ofs_bitmap = self._io.read_s4le()
            self._debug['ofs_bitmap']['end'] = self._io.pos()


    class BitmapInfo(KaitaiStruct):
        """
        .. seealso::
           Source - https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapinfo
        """
        SEQ_FIELDS = ["len_header", "header", "color_mask", "color_table"]
        def __init__(self, _io, _parent=None, _root=None):
            self._io = _io
            self._parent = _parent
            self._root = _root if _root else self
            self._debug = collections.defaultdict(dict)

        def _read(self):
            self._debug['len_header']['start'] = self._io.pos()
            self.len_header = self._io.read_u4le()
            self._debug['len_header']['end'] = self._io.pos()
            self._debug['header']['start'] = self._io.pos()
            self._raw_header = self._io.read_bytes((self.len_header - 4))
            _io__raw_header = KaitaiStream(BytesIO(self._raw_header))
            self.header = Bmp.BitmapHeader(self.len_header, _io__raw_header, self, self._root)
            self.header._read()
            self._debug['header']['end'] = self._io.pos()
            if  ((not (self._io.is_eof())) and (self.is_color_mask_here)) :
                self._debug['color_mask']['start'] = self._io.pos()
                self.color_mask = Bmp.ColorMask(self.header.bitmap_info_ext.compression == Bmp.Compressions.alpha_bitfields, self._io, self, self._root)
                self.color_mask._read()
                self._debug['color_mask']['end'] = self._io.pos()

            if not (self._io.is_eof()):
                self._debug['color_table']['start'] = self._io.pos()
                self._raw_color_table = self._io.read_bytes_full()
                _io__raw_color_table = KaitaiStream(BytesIO(self._raw_color_table))
                self.color_table = Bmp.ColorTable(not (self.header.is_core_header), (self.header.bitmap_info_ext.num_colors_used if self.header.extends_bitmap_info else 0), _io__raw_color_table, self, self._root)
                self.color_table._read()
                self._debug['color_table']['end'] = self._io.pos()


        @property
        def is_color_mask_given(self):
            if hasattr(self, '_m_is_color_mask_given'):
                return self._m_is_color_mask_given if hasattr(self, '_m_is_color_mask_given') else None

            self._m_is_color_mask_given =  ((self.header.extends_bitmap_info) and ( ((self.header.bitmap_info_ext.compression == Bmp.Compressions.bitfields) or (self.header.bitmap_info_ext.compression == Bmp.Compressions.alpha_bitfields)) ) and ( ((self.is_color_mask_here) or (self.header.is_color_mask_here)) )) 
            return self._m_is_color_mask_given if hasattr(self, '_m_is_color_mask_given') else None

        @property
        def color_mask_given(self):
            if hasattr(self, '_m_color_mask_given'):
                return self._m_color_mask_given if hasattr(self, '_m_color_mask_given') else None

            if self.is_color_mask_given:
                self._m_color_mask_given = (self.color_mask if self.is_color_mask_here else self.header.color_mask)

            return self._m_color_mask_given if hasattr(self, '_m_color_mask_given') else None

        @property
        def color_mask_blue(self):
            if hasattr(self, '_m_color_mask_blue'):
                return self._m_color_mask_blue if hasattr(self, '_m_color_mask_blue') else None

            self._m_color_mask_blue = (self.color_mask_given.blue_mask if self.is_color_mask_given else (31 if self.header.bits_per_pixel == 16 else (255 if  ((self.header.bits_per_pixel == 24) or (self.header.bits_per_pixel == 32))  else 0)))
            return self._m_color_mask_blue if hasattr(self, '_m_color_mask_blue') else None

        @property
        def color_mask_alpha(self):
            if hasattr(self, '_m_color_mask_alpha'):
                return self._m_color_mask_alpha if hasattr(self, '_m_color_mask_alpha') else None

            self._m_color_mask_alpha = (self.color_mask_given.alpha_mask if  ((self.is_color_mask_given) and (self.color_mask_given.has_alpha_mask))  else 0)
            return self._m_color_mask_alpha if hasattr(self, '_m_color_mask_alpha') else None

        @property
        def color_mask_green(self):
            if hasattr(self, '_m_color_mask_green'):
                return self._m_color_mask_green if hasattr(self, '_m_color_mask_green') else None

            self._m_color_mask_green = (self.color_mask_given.green_mask if self.is_color_mask_given else (992 if self.header.bits_per_pixel == 16 else (65280 if  ((self.header.bits_per_pixel == 24) or (self.header.bits_per_pixel == 32))  else 0)))
            return self._m_color_mask_green if hasattr(self, '_m_color_mask_green') else None

        @property
        def is_color_mask_here(self):
            if hasattr(self, '_m_is_color_mask_here'):
                return self._m_is_color_mask_here if hasattr(self, '_m_is_color_mask_here') else None

            self._m_is_color_mask_here =  ((self.header.len_header == Bmp.HeaderType.bitmap_info_header.value) and ( ((self.header.bitmap_info_ext.compression == Bmp.Compressions.bitfields) or (self.header.bitmap_info_ext.compression == Bmp.Compressions.alpha_bitfields)) )) 
            return self._m_is_color_mask_here if hasattr(self, '_m_is_color_mask_here') else None

        @property
        def color_mask_red(self):
            if hasattr(self, '_m_color_mask_red'):
                return self._m_color_mask_red if hasattr(self, '_m_color_mask_red') else None

            self._m_color_mask_red = (self.color_mask_given.red_mask if self.is_color_mask_given else (31744 if self.header.bits_per_pixel == 16 else (16711680 if  ((self.header.bits_per_pixel == 24) or (self.header.bits_per_pixel == 32))  else 0)))
            return self._m_color_mask_red if hasattr(self, '_m_color_mask_red') else None



