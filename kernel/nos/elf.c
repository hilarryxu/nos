#include <nos/elf.h>

#include <nos/proc/process.h>

static int
load_by_prog_header(const char *elf_data, size_t size,
                    struct elf_prog_header *ph, struct process *proc)
{
}

static int
load_by_prog_headers(const char *elf_data, size_t size,
                     struct elf_prog_header *ph, uint32_t ph_struct_size,
                     uint32_t num, struct process *proc)
{
  for (int i = 0; i < num; i++, ph += ph_struct_size) {
    if (ph->p_type == ELF_PROG_LOAD) {
      if (load_by_prog_header(elf_data, size, ph, proc) != NOS_OK)
        return -1;
    }
  }

  return NOS_OK;
}
int
load_elf_program(struct process *proc, const char *elf_data, size_t size)
{
  int rc;
  const struct elf_header *elf_hdr = (const struct elf_header *)elf_data;
  uint32_t phentsize = elf_hdr->e_phentsize;
  uint32_t phnum = elf_hdr->e_phnum;

  if (elf_hdr->e_magic != ELF_MAGIC)
    return -1;
  if (elf_hdr->e_phoff == 0 || elf_hdr->e_phoff >= size)
    return -1;

  rc = load_by_prog_headers(
      elf_data, size,
      (struct elf_prog_header *)((uint8_t *)elf_data + elfhdr->e_phoff),
      phentsize, phnum, proc);
  if (rc != NOS_OK)
    return -1;

  proc->entry = elf_hdr->e_entry;
  return NOS_OK;
}
