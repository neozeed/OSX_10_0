#include <ctype.h>

#include "pef.h"
#include "pef-traceback.h"

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"

#define bfd_pef_close_and_cleanup _bfd_generic_close_and_cleanup
#define bfd_pef_bfd_free_cached_info _bfd_generic_bfd_free_cached_info
#define bfd_pef_new_section_hook _bfd_generic_new_section_hook
#define bfd_pef_bfd_is_local_label_name bfd_generic_is_local_label_name
#define bfd_pef_get_lineno _bfd_nosymbols_get_lineno
#define bfd_pef_find_nearest_line _bfd_nosymbols_find_nearest_line
#define bfd_pef_bfd_make_debug_symbol _bfd_nosymbols_bfd_make_debug_symbol
#define bfd_pef_read_minisymbols _bfd_generic_read_minisymbols
#define bfd_pef_minisymbol_to_symbol _bfd_generic_minisymbol_to_symbol

#define bfd_pef_get_reloc_upper_bound _bfd_norelocs_get_reloc_upper_bound
#define bfd_pef_canonicalize_reloc _bfd_norelocs_canonicalize_reloc
#define bfd_pef_bfd_reloc_type_lookup _bfd_norelocs_bfd_reloc_type_lookup

#define bfd_pef_set_arch_mach _bfd_generic_set_arch_mach

#define bfd_pef_get_section_contents _bfd_generic_get_section_contents
#define bfd_pef_set_section_contents _bfd_generic_set_section_contents

#define bfd_pef_bfd_get_relocated_section_contents \
  bfd_generic_get_relocated_section_contents
#define bfd_pef_bfd_relax_section bfd_generic_relax_section
#define bfd_pef_bfd_gc_sections bfd_generic_gc_sections
#define bfd_pef_bfd_link_hash_table_create _bfd_generic_link_hash_table_create
#define bfd_pef_bfd_link_add_symbols _bfd_generic_link_add_symbols
#define bfd_pef_bfd_final_link _bfd_generic_final_link
#define bfd_pef_bfd_link_split_section _bfd_generic_link_split_section
#define bfd_pef_get_section_contents_in_window \
  _bfd_generic_get_section_contents_in_window

static boolean bfd_pef_mkobject PARAMS ((bfd *));
static const bfd_target *bfd_pef_object_p PARAMS ((bfd *));
static long bfd_pef_get_symtab_upper_bound PARAMS ((bfd *));
static long bfd_pef_get_symtab PARAMS ((bfd *, asymbol **));
static asymbol *bfd_pef_make_empty_symbol PARAMS ((bfd *));
static void bfd_pef_get_symbol_info PARAMS ((bfd *, asymbol *, symbol_info *));
static int bfd_pef_sizeof_headers PARAMS ((bfd *, boolean));

static void
bfd_pef_print_symbol (ignore_abfd, afile, symbol, how)
     bfd *ignore_abfd;
     PTR afile;
     asymbol *symbol;
     bfd_print_symbol_type how;
{
  FILE *file = (FILE *) afile;
  switch (how) {
  case bfd_print_symbol_name:
    fprintf (file, "%s", symbol->name);
    break;
  default:
    bfd_print_symbol_vandf ((PTR) file, symbol);
    fprintf (file, " %-5s %s", symbol->section->name, symbol->name);
  }
}

static boolean
bfd_pef_mkobject (abfd)
     bfd *abfd ATTRIBUTE_UNUSED;
{
  return true;
}

static int
bfd_pef_parse_traceback_table (abfd, section, buf, len, pos, sym)
     bfd *abfd;
     asection *section;
     unsigned char *buf;
     size_t len;
     size_t pos;
     asymbol *sym;
{
  struct traceback_table table;
  size_t offset;
  const char *s;

  sym->name = NULL;
  sym->value = 0;
  sym->the_bfd = abfd;
  sym->section = section;
  sym->flags = 0;
  sym->udata.i = 0;

#if 0
  if ((pos + 4) > len) { return -1; }
  if (bfd_getb32 (buf + pos) != 0) { return -1; }
#endif

  /* memcpy is fine since all fields are unsigned char */

  if ((pos + 8) > len) { return -1; }
  memcpy (&table, buf + pos, 8);
  
  /* calling code relies on returned symbols having a name and correct offset */

  if ((table.lang != TB_C) && (table.lang != TB_CPLUSPLUS)) { 
    return -1;
  }
  if (! (table.flags2 & TB_NAME_PRESENT)) {
    return -1;
  }
  if (! table.flags1 & TB_HAS_TBOFF) {
    return -1;
  }

  offset = 8;

  if ((table.flags5 & TB_FLOATPARAMS) || (table.fixedparams)) {
    offset += 4; 
  }

  if (table.flags1 & TB_HAS_TBOFF) {

    struct traceback_table_tboff off;
    
    if ((pos + offset + 4) > len) { return -1; }
    off.tb_offset = bfd_getb32 (buf + pos + offset);
    offset += 4;

    /* need to subtract 4 because the offset includes the 0x0L
       preceding the table */

    if ((off.tb_offset + 4) > pos) {
      return -1;
    }
    sym->value = pos - off.tb_offset - 4;
  }

  if (table.flags2 & TB_INT_HNDL) {
    offset += 4;
  }

  if (table.flags1 & TB_HAS_CTL) {

    struct traceback_table_anchors anchors;

    if ((pos + offset + 4) > len) { return -1; }
    anchors.ctl_info = bfd_getb32 (buf + pos + offset);
    offset += 4;

    if (anchors.ctl_info > 1024) { 
      return -1;
    }

    offset += anchors.ctl_info * 4;
  }

  if (table.flags2 & TB_NAME_PRESENT) {

    struct traceback_table_routine name;
    char *namebuf;

    if ((pos + offset + 2) > len) { return -1; }
    name.name_len = bfd_getb16 (buf + pos + offset);
    offset += 2;

    if (name.name_len > 4096) { return -1; }

    if ((pos + offset + name.name_len) > len) { return -1; }

    namebuf = (char *) bfd_alloc (abfd, name.name_len + 1);
    if (namebuf == NULL) { return -1; }

    memcpy (namebuf, buf + pos + offset, name.name_len);
    namebuf[name.name_len] = '\0';
    
    /* strip leading period inserted by compiler */
    if (namebuf[0] == '.') {
      memmove (namebuf, namebuf + 1, name.name_len + 1);
    }

    sym->name = namebuf;

    for (s = sym->name; (*s != '\0'); s++) {
      if (! isprint (*s)) {
	return -1;
      }
    }
  }

  if (table.flags2 & TB_USES_ALLOCA) {
    offset += 4;
  }

  if (table.flags4 & TB_HAS_VEC_INFO) {
    offset += 4;
  }

  return offset;
}

static const char *bfd_pef_section_name (section)
     bfd_pef_section *section;
{
  switch (section->section_kind) {
  case BFD_PEF_SECTION_CODE: return "code";
  case BFD_PEF_SECTION_UNPACKED_DATA: return "unpacked-data";
  case BFD_PEF_SECTION_PACKED_DATA: return "packed-data";
  case BFD_PEF_SECTION_CONSTANT: return "constant";
  case BFD_PEF_SECTION_LOADER: return "loader";
  case BFD_PEF_SECTION_DEBUG: return "debug";
  case BFD_PEF_SECTION_EXEC_DATA: return "exec-data";
  case BFD_PEF_SECTION_EXCEPTION: return "exception";
  case BFD_PEF_SECTION_TRACEBACK: return "traceback";
  default: return "unknown";
  }
}

static asection *
bfd_pef_make_bfd_section (abfd, section)
     bfd *abfd;
     bfd_pef_section *section;
{
  asection *bfdsec;
  const char *name = bfd_pef_section_name (section);

  bfdsec = bfd_make_section_anyway (abfd, name);
  if (bfdsec == NULL) { return NULL; }
  
  bfdsec->vma = section->default_address + section->container_offset;
  bfdsec->lma = section->default_address + section->container_offset;
  bfdsec->_raw_size = section->container_length;
  bfdsec->filepos = section->container_offset;
  bfdsec->alignment_power = section->alignment;

  bfdsec->flags = SEC_HAS_CONTENTS | SEC_LOAD | SEC_ALLOC | SEC_CODE;

  return bfdsec;
}

int bfd_pef_parse_loader_header (abfd, buf, len, header)
     bfd *abfd;
     unsigned char *buf;
     size_t len;
     bfd_pef_loader_header *header;
{
  BFD_ASSERT (len == 56);

  header->main_section = bfd_getb32 (buf);
  header->main_offset = bfd_getb32 (buf + 4);
  header->init_section = bfd_getb32 (buf + 8);
  header->init_offset = bfd_getb32 (buf + 12);
  header->term_section = bfd_getb32 (buf + 16);
  header->term_offset = bfd_getb32 (buf + 20);
  header->imported_library_count = bfd_getb32 (buf + 24);
  header->total_imported_symbol_count = bfd_getb32 (buf + 28);
  header->reloc_section_count = bfd_getb32 (buf + 32);
  header->reloc_instr_offset = bfd_getb32 (buf + 36);
  header->loader_strings_offset = bfd_getb32 (buf + 40);
  header->export_hash_offset = bfd_getb32 (buf + 44);
  header->export_hash_table_power = bfd_getb32 (buf + 48);
  header->exported_symbol_count = bfd_getb32 (buf + 52);

  return 0;
}

int bfd_pef_parse_imported_library (abfd, buf, len, header)
     bfd *abfd;
     unsigned char *buf;
     size_t len;
     bfd_pef_imported_library *header;
{
  BFD_ASSERT (len == 24);

  header->name_offset = bfd_getb32 (buf);
  header->old_implementation_version = bfd_getb32 (buf + 4);
  header->current_version = bfd_getb32 (buf + 8);
  header->imported_symbol_count = bfd_getb32 (buf + 12);
  header->first_imported_symbol = bfd_getb32 (buf + 16);
  header->options = buf[20];
  header->reserved_a = buf[21];
  header->reserved_b = bfd_getb16 (buf + 22);

  return 0;
}

int bfd_pef_parse_imported_symbol (abfd, buf, len, symbol)
     bfd *abfd;
     unsigned char *buf;
     size_t len;
     bfd_pef_imported_symbol *symbol;
{
  unsigned long value;

  BFD_ASSERT (len == 4);

  value = bfd_getb32 (buf);
  symbol->class = value >> 24;
  symbol->name = value & 0x00ffffff;

  return 0;
}

int bfd_pef_scan_section (abfd, section)
     bfd *abfd;
     bfd_pef_section *section;
{
  unsigned char buf[28];
  
  bfd_seek (abfd, section->header_offset, SEEK_SET);
  if (bfd_read ((PTR) buf, 1, 28, abfd) != 28) { return -1; }

  section->name_offset = bfd_h_get_32 (abfd, buf);
  section->default_address = bfd_h_get_32 (abfd, buf + 4);
  section->total_length = bfd_h_get_32 (abfd, buf + 8);
  section->unpacked_length = bfd_h_get_32 (abfd, buf + 12);
  section->container_length = bfd_h_get_32 (abfd, buf + 16);
  section->container_offset = bfd_h_get_32 (abfd, buf + 20);
  section->section_kind = buf[24];
  section->share_kind = buf[25];
  section->alignment = buf[26];
  section->reserved = buf[27];

  section->bfd_section = bfd_pef_make_bfd_section (abfd, section);
  if (section->bfd_section == NULL) { return -1; }

  return 0;
}

int
bfd_pef_scan (abfd, header)
     bfd *abfd;
     bfd_pef_header *header;
{
  unsigned int i;
  bfd_pef_data_struct *mdata = NULL;

  if ((header->tag1 != BFD_PEF_TAG1) || (header->tag2 != BFD_PEF_TAG2)) {
    return -1;
  }

  mdata = ((bfd_pef_data_struct *) 
	   bfd_alloc (abfd, sizeof (bfd_pef_data_struct)));
  if (mdata == NULL) { return -1; }
  
  mdata->header = *header;

  abfd->flags = abfd->xvec->object_flags | (abfd->flags & (BFD_IN_MEMORY | BFD_IO_FUNCS));

  if (header->section_count != 0) {
    
    mdata->sections = 
      ((bfd_pef_section *)
       bfd_alloc (abfd, header->section_count * sizeof (bfd_pef_section)));
    if (mdata->sections == NULL) { return -1; } 

    for (i = 0; i < header->section_count; i++) {
      bfd_pef_section *cur = &mdata->sections[i];
      cur->header_offset = 40 + (i * 28);
      if (bfd_pef_scan_section (abfd, cur) < 0) {
	return -1;
      }
    }
  }

  abfd->tdata.pef_data = mdata;

  return 0;
}

static int
bfd_pef_read_header (abfd, header)
     bfd *abfd;
     bfd_pef_header *header;
{
  unsigned char buf[40];

  bfd_seek (abfd, 0, SEEK_SET);

  if (bfd_read ((PTR) buf, 1, 40, abfd) != 40) { return -1; }

  header->tag1 = bfd_getb32 (buf);
  header->tag2 = bfd_getb32 (buf + 4);
  header->architecture = bfd_getb32 (buf + 8);
  header->format_version = bfd_getb32 (buf + 12);
  header->timestamp = bfd_getb32 (buf + 16);
  header->old_definition_version = bfd_getb32 (buf + 20);
  header->old_implementation_version = bfd_getb32 (buf + 24);
  header->current_version = bfd_getb32 (buf + 28);
  header->section_count = bfd_getb32 (buf + 32) + 1;
  header->instantiated_section_count = bfd_getb32 (buf + 34);
  header->reserved = bfd_getb32 (buf + 36);

  return 0;
}

static const bfd_target *
bfd_pef_object_p (abfd)
     bfd *abfd;
{
  bfd_pef_header header;
  
  abfd->tdata.pef_data = NULL;

  if (bfd_pef_read_header (abfd, &header) != 0) {
    abfd->tdata.pef_data = NULL;
    bfd_set_error (bfd_error_wrong_format);
    return NULL;
  }

  if (bfd_pef_scan (abfd, &header) != 0) {
    abfd->tdata.pef_data = NULL;
    bfd_set_error (bfd_error_wrong_format);
    return NULL;
  }

  return abfd->xvec;
}

static long bfd_pef_parse_traceback_tables (abfd, sec, buf, len, csym)
     bfd *abfd;
     asection *sec;
     unsigned char *buf;
     size_t len;
     asymbol **csym;
{
  asymbol function;
  asymbol traceback;

  const char *const tbprefix = "__traceback_";
  size_t tbnamelen;

  size_t pos = 0;
  unsigned long count = 0;
  int ret;

  for (;;) {
	
    /* we're reading symbols two at a time */

    if (csym && ((csym[count] == NULL) || (csym[count + 1] == NULL))) {
      break;
    }

    pos += 3;
    pos -= (pos % 4);

    while ((pos + 4) <= len) {
      if (bfd_getb32 (buf + pos) == 0) {
	break;
      }
      pos += 4;
    }

    if ((pos + 4) > len) {
      break;
    }
	
    ret = bfd_pef_parse_traceback_table (abfd, sec, buf, len, pos + 4, &function);
    if (ret < 0) {
      /* skip over 0x0L to advance to next possible traceback table */
      pos += 4;
      continue;
    }
	
    BFD_ASSERT (function.name != NULL);

    tbnamelen = strlen (tbprefix) + strlen (function.name);
    traceback.name = bfd_alloc (abfd, tbnamelen + 1);
    if (traceback.name == NULL) { 
      bfd_release (abfd, function.name);
      break;
    }
    snprintf (traceback.name, tbnamelen + 1, "%s%s", tbprefix, function.name);

    traceback.value = pos;
    traceback.the_bfd = abfd;
    traceback.section = sec;
    traceback.flags = 0;
    traceback.udata.i = 0;
	
    pos += ret;
	
    if (csym != NULL) {
      *(csym[count]) = function;
      *(csym[count + 1]) = traceback;
    }
    count += 2;
  }

  return count;
}

static int bfd_pef_parse_function_stub (abfd, buf, len, offset)
     bfd *abfd;
     unsigned char *buf;
     size_t len;
     unsigned long *offset;
{
  BFD_ASSERT (len == 24);

  if ((bfd_getb32 (buf) & 0xffff0000) != 0x81820000) { return -1; }
  if (bfd_getb32 (buf + 4) != 0x90410014) { return -1; }
  if (bfd_getb32 (buf + 8) != 0x800c0000) { return -1; }
  if (bfd_getb32 (buf + 12) != 0x804c0004) { return -1; }
  if (bfd_getb32 (buf + 16) != 0x7c0903a6) { return -1; }
  if (bfd_getb32 (buf + 20) != 0x4e800420) { return -1; }
  
  if (offset != NULL) {
    *offset = (bfd_getb32 (buf) & 0x0000ffff) / 4;
  }

  return 0;
}

static long bfd_pef_parse_function_stubs (abfd, codesec, codebuf, codelen, loaderbuf, loaderlen, csym)
     bfd *abfd;
     asection *codesec;
     unsigned char *codebuf;
     size_t codelen;
     unsigned char *loaderbuf;
     size_t loaderlen;
     asymbol **csym;
{
  const char *const sprefix = "__stub_";
  size_t snamelen;

  size_t codepos = 0;
  unsigned long count = 0;

  bfd_pef_loader_header header;
  bfd_pef_imported_library *libraries;
  bfd_pef_imported_symbol *imports;

  unsigned long i;
  int ret;

  ret = bfd_pef_parse_loader_header (abfd, loaderbuf, 56, &header);
  if (ret < 0) { goto end; }

  libraries = (bfd_pef_imported_library *) xmalloc
    (header.imported_library_count * sizeof (bfd_pef_imported_library));
  imports = (bfd_pef_imported_symbol *) xmalloc
    (header.total_imported_symbol_count * sizeof (bfd_pef_imported_symbol));
  
  for (i = 0; i < header.imported_library_count; i++) {
    ret = bfd_pef_parse_imported_library
      (abfd, loaderbuf + 56 + (i * 24), 24, &libraries[i]);
  }
  
  for (i = 0; i < header.total_imported_symbol_count; i++) {
    ret = bfd_pef_parse_imported_symbol 
      (abfd, loaderbuf + 56 + (header.imported_library_count * 24) + (i * 4), 4, &imports[i]);
  }
    
  codepos = 0;

  for (;;) {

    asymbol sym;
    const char *name;
    unsigned long index;
    int ret;

    if (csym && (csym[count] == NULL)) { break; }

    codepos += 3;
    codepos -= (codepos % 4);

    while ((codepos + 4) <= codelen) {
      if ((bfd_getb32 (codebuf + codepos) & 0xffff0000) == 0x81820000) { 
	break;
      }
      codepos += 4;
    }

    if ((codepos + 4) > codelen) {
      break;
    }

    ret = bfd_pef_parse_function_stub (abfd, codebuf + codepos, 24, &index);
    if (ret < 0) { codepos += 24; continue; }
    
    if (index >= header.total_imported_symbol_count) { codepos += 24; continue; }

    name = loaderbuf + header.loader_strings_offset + imports[index].name;

    snamelen = strlen (sprefix) + strlen (name);
    sym.name = bfd_alloc (abfd, snamelen + 1);
    if (sym.name == NULL) { break; }
    snprintf (sym.name, snamelen + 1, "%s%s", sprefix, name);

    sym.value = codepos;
    sym.the_bfd = abfd;
    sym.section = codesec;
    sym.flags = 0;
    sym.udata.i = 0;

    codepos += 24;

    if (csym != NULL) {
      *(csym[count]) = sym;
    }
    count++;
  }  

 end:
  if (libraries != NULL) { xfree (libraries); }
  if (imports != NULL) { xfree (imports); }

  return count;
}   

static long bfd_pef_parse_symbols (abfd, csym)
     bfd *abfd;
     asymbol **csym;
{
  unsigned long count = 0;

  asection *codesec = NULL;
  unsigned char *codebuf;
  size_t codelen;

  asection *loadersec = NULL;
  unsigned char *loaderbuf;
  size_t loaderlen;

  codesec = bfd_get_section_by_name (abfd, "code");
  loadersec = bfd_get_section_by_name (abfd, "loader");

  codelen = bfd_section_size (abfd, codesec);
  loaderlen = bfd_section_size (abfd, loadersec);

  codebuf = (unsigned char *) xmalloc (codelen);
  loaderbuf = (unsigned char *) xmalloc (loaderlen);

  if (bfd_seek (abfd, codesec->filepos, SEEK_SET) < 0) { goto end; }
  if (bfd_read ((PTR) codebuf, 1, codelen, abfd) != codelen) { goto end; }

  if (bfd_seek (abfd, loadersec->filepos, SEEK_SET) < 0) { goto end; }
  if (bfd_read ((PTR) loaderbuf, 1, loaderlen, abfd) != loaderlen) { goto end; }

  count = 0;
  count += bfd_pef_parse_traceback_tables (abfd, codesec, codebuf, codelen, csym);
  count += bfd_pef_parse_function_stubs
    (abfd, codesec, codebuf, codelen, loaderbuf, loaderlen, (csym != NULL) ? (csym + count) : NULL);
    
  if (csym != NULL) {
    csym[count] = NULL;
  }
  
 end:
  if (codebuf != NULL) { xfree (codebuf); }
  if (loaderbuf != NULL) { xfree (loaderbuf); }
  
  return count;
}

static long
bfd_pef_count_symbols (abfd)
     bfd *abfd;
{
  return bfd_pef_parse_symbols (abfd, NULL);
}

static long
bfd_pef_get_symtab_upper_bound (abfd)
     bfd *abfd;
{
  long nsyms = bfd_pef_count_symbols (abfd);
  if (nsyms < 0) { return nsyms; }
  return ((nsyms + 1) * sizeof (asymbol *));
}

static long
bfd_pef_get_symtab (abfd, alocation)
     bfd *abfd;
     asymbol **alocation;
{
  long i;
  asymbol *syms;
  long ret;

  long nsyms = bfd_pef_count_symbols (abfd);
  if (nsyms < 0) { return nsyms; }

  syms = bfd_alloc (abfd, nsyms * sizeof (asymbol));
  if (syms == NULL) { return -1; }

  for (i = 0; i < nsyms; i++) {
    alocation[i] = &syms[i];
  }
  alocation[nsyms] = NULL;

  ret = bfd_pef_parse_symbols (abfd, alocation);
  if (ret != nsyms) {
    return 0;
  }

  return ret;
}

static asymbol *
bfd_pef_make_empty_symbol (abfd)
     bfd *abfd;
{
  return (asymbol *) bfd_alloc (abfd, sizeof (asymbol));
}

static void
bfd_pef_get_symbol_info (ignore_abfd, symbol, ret)
     bfd *ignore_abfd ATTRIBUTE_UNUSED;
     asymbol *symbol;
     symbol_info *ret;
{
  bfd_symbol_info (symbol, ret);
}

static int
bfd_pef_sizeof_headers (abfd, exec)
     bfd *abfd ATTRIBUTE_UNUSED;
     boolean exec ATTRIBUTE_UNUSED;
{
  return 0;
}

const bfd_target pef_vec =
{
  "pef",			/* name */
  bfd_target_pef_flavour,	/* flavour */
  BFD_ENDIAN_BIG,		/* byteorder */
  BFD_ENDIAN_BIG,		/* header_byteorder */
  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT | D_PAGED),
  (SEC_ALLOC | SEC_LOAD | SEC_READONLY | SEC_CODE | SEC_DATA
   | SEC_ROM | SEC_HAS_CONTENTS), /* section_flags */
  0,				/* symbol_leading_char */
  ' ',				/* ar_pad_char */
  16,				/* ar_max_namelen */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
  bfd_getb32, bfd_getb_signed_32, bfd_putb32,
  bfd_getb16, bfd_getb_signed_16, bfd_putb16,	/* data */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
  bfd_getb32, bfd_getb_signed_32, bfd_putb32,
  bfd_getb16, bfd_getb_signed_16, bfd_putb16,	/* hdrs */
  {				/* bfd_check_format */
    _bfd_dummy_target,
    bfd_pef_object_p,		/* bfd_check_format */
    _bfd_dummy_target,
    _bfd_dummy_target,
  },
  {				/* bfd_set_format */
    bfd_false,
    bfd_pef_mkobject,
    bfd_false,
    bfd_false,
  },
  {				/* bfd_write_contents */
    bfd_false,
    bfd_true,
    bfd_false,
    bfd_false,
  },

  BFD_JUMP_TABLE_GENERIC (bfd_pef),
  BFD_JUMP_TABLE_COPY (_bfd_generic),
  BFD_JUMP_TABLE_CORE (_bfd_nocore),
  BFD_JUMP_TABLE_ARCHIVE (_bfd_noarchive),
  BFD_JUMP_TABLE_SYMBOLS (bfd_pef),
  BFD_JUMP_TABLE_RELOCS (bfd_pef),
  BFD_JUMP_TABLE_WRITE (bfd_pef),
  BFD_JUMP_TABLE_LINK (bfd_pef),
  BFD_JUMP_TABLE_DYNAMIC (_bfd_nodynamic),

  NULL,
  
  NULL
};

#define bfd_pef_xlib_close_and_cleanup _bfd_generic_close_and_cleanup
#define bfd_pef_xlib_bfd_free_cached_info _bfd_generic_bfd_free_cached_info
#define bfd_pef_xlib_new_section_hook _bfd_generic_new_section_hook
#define bfd_pef_xlib_get_section_contents _bfd_generic_get_section_contents
#define bfd_pef_xlib_set_section_contents _bfd_generic_set_section_contents
#define bfd_pef_xlib_get_section_contents_in_window _bfd_generic_get_section_contents_in_window
#define bfd_pef_xlib_set_section_contents_in_window _bfd_generic_set_section_contents_in_window

static int
bfd_pef_xlib_read_header (abfd, header)
     bfd *abfd;
     bfd_pef_xlib_header *header;
{
  unsigned char buf[76];

  bfd_seek (abfd, 0, SEEK_SET);

  if (bfd_read ((PTR) buf, 1, 76, abfd) != 76) { return -1; }

  header->tag1 = bfd_getb32 (buf);
  header->tag2 = bfd_getb32 (buf + 4);
  header->current_format = bfd_getb32 (buf + 8);
  header->container_strings_offset = bfd_getb32 (buf + 12);
  header->export_hash_offset = bfd_getb32 (buf + 16);
  header->export_key_offset = bfd_getb32 (buf + 20);
  header->export_symbol_offset = bfd_getb32 (buf + 24);
  header->export_names_offset = bfd_getb32 (buf + 28);
  header->export_hash_table_power = bfd_getb32 (buf + 32);
  header->exported_symbol_count = bfd_getb32 (buf + 36);
  header->frag_name_offset = bfd_getb32 (buf + 40);
  header->frag_name_length = bfd_getb32 (buf + 44);
  header->dylib_path_offset = bfd_getb32 (buf + 48);
  header->dylib_path_length = bfd_getb32 (buf + 52);
  header->cpu_family = bfd_getb32 (buf + 56);
  header->cpu_model = bfd_getb32 (buf + 60);
  header->date_time_stamp = bfd_getb32 (buf + 64);
  header->current_version = bfd_getb32 (buf + 68);
  header->old_definition_version = bfd_getb32 (buf + 72);
  header->old_implementation_version = bfd_getb32 (buf + 76);

  return 0;
}

int
bfd_pef_xlib_scan (abfd, header)
     bfd *abfd;
     bfd_pef_xlib_header *header;
{
  bfd_pef_xlib_data_struct *mdata = NULL;

  if ((header->tag1 != BFD_PEF_XLIB_TAG1) 
      || ((header->tag2 != BFD_PEF_VLIB_TAG2) && (header->tag2 != BFD_PEF_BLIB_TAG2))) {
    return -1;
  }

  mdata = ((bfd_pef_xlib_data_struct *) 
	   bfd_alloc (abfd, sizeof (bfd_pef_xlib_data_struct)));
  if (mdata == NULL) { return -1; }
  
  mdata->header = *header;

  abfd->flags = abfd->xvec->object_flags | (abfd->flags & (BFD_IN_MEMORY | BFD_IO_FUNCS));

  abfd->tdata.pef_xlib_data = mdata;

  return 0;
}

static const bfd_target *
bfd_pef_xlib_object_p (abfd)
     bfd *abfd;
{
  bfd_pef_xlib_header header;
  
  abfd->tdata.pef_xlib_data = NULL;

  if (bfd_pef_xlib_read_header (abfd, &header) != 0) {
    abfd->tdata.pef_xlib_data = NULL;
    bfd_set_error (bfd_error_wrong_format);
    return NULL;
  }

  if (bfd_pef_xlib_scan (abfd, &header) != 0) {
    abfd->tdata.pef_xlib_data = NULL;
    bfd_set_error (bfd_error_wrong_format);
    return NULL;
  }

  return abfd->xvec;
}

const bfd_target pef_xlib_vec =
{
  "pef-xlib",			/* name */
  bfd_target_pef_xlib_flavour,	/* flavour */
  BFD_ENDIAN_BIG,		/* byteorder */
  BFD_ENDIAN_BIG,		/* header_byteorder */
  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT | D_PAGED),
  (SEC_ALLOC | SEC_LOAD | SEC_READONLY | SEC_CODE | SEC_DATA
   | SEC_ROM | SEC_HAS_CONTENTS), /* section_flags */
  0,				/* symbol_leading_char */
  ' ',				/* ar_pad_char */
  16,				/* ar_max_namelen */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
  bfd_getb32, bfd_getb_signed_32, bfd_putb32,
  bfd_getb16, bfd_getb_signed_16, bfd_putb16,	/* data */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
  bfd_getb32, bfd_getb_signed_32, bfd_putb32,
  bfd_getb16, bfd_getb_signed_16, bfd_putb16,	/* hdrs */
  {				/* bfd_check_format */
    _bfd_dummy_target,
    bfd_pef_xlib_object_p,	/* bfd_check_format */
    _bfd_dummy_target,
    _bfd_dummy_target,
  },
  {				/* bfd_set_format */
    bfd_false,
    bfd_pef_mkobject,
    bfd_false,
    bfd_false,
  },
  {				/* bfd_write_contents */
    bfd_false,
    bfd_true,
    bfd_false,
    bfd_false,
  },

  BFD_JUMP_TABLE_GENERIC (bfd_pef_xlib),
  BFD_JUMP_TABLE_COPY (_bfd_generic),
  BFD_JUMP_TABLE_CORE (_bfd_nocore),
  BFD_JUMP_TABLE_ARCHIVE (_bfd_noarchive),
  BFD_JUMP_TABLE_SYMBOLS (_bfd_nosymbols),
  BFD_JUMP_TABLE_RELOCS (_bfd_norelocs),
  BFD_JUMP_TABLE_WRITE (_bfd_nowrite),
  BFD_JUMP_TABLE_LINK (_bfd_nolink),
  BFD_JUMP_TABLE_DYNAMIC (_bfd_nodynamic),

  NULL,
  
  NULL
};
