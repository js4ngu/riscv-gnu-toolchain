# Generate the main loop of the simulator.
# Copyright (C) 1996, 1997, 1998 Free Software Foundation, Inc.
# Contributed by Cygnus Support.
#
# This file is part of the GNU simulators.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
# This file creates two files: eng.hin and mloop.cin.
# eng.hin defines a few macros that specify what kind of engine was selected
# based on the arguments to this script.
# mloop.cin contains the engine.
#
# ??? Rename mloop.c to eng.c?
# ??? Rename mainloop.in to engine.in?
# ??? Rename this file to genengine.sh?
#
# Syntax: genmloop.sh [options]
#
# Options:
#
# -mono | -multi
#    - specify single cpu or multiple cpus (number specifyable at runtime),
#      maximum number is a configuration parameter
#    - -multi wip
#
# -fast: include support for fast execution in addition to full featured mode
#
#	Full featured mode is for tracing, profiling, etc. and is always
#	provided.  Fast mode contains no frills, except speed.
#	A target need only provide a "full" version of one of
#	simple,scache,pbb.  If the target wants it can also provide a fast
#	version of same.  It can't provide more than this.
#	??? Later add ability to have another set of full/fast semantics
#	for use in with-devices/with-smp situations (pbb can be inappropriate
#	here).
#
# -full-switch: same as -fast but for full featured version of -switch
#	Only needed if -fast present.
#
# -simple: simple execution engine (the default)
#
#	This engine fetches and executes one instruction at a time.
#	??? The implementation is currently slower than necessary for
#	simplicity.  Instead of storing extract insn fields in ARGBUF,
#	they should just be extracted from the insn when needed.
#
# -scache: use the scache to speed things up (not always a win)
#
#	This engine caches the extracted instruction before executing it.
#	When executing instructions they are first looked up in the scache.
#
# -pbb: same as -scache but extract a (pseudo-) basic block at a time
#
#	This engine is basically identical to the scache version except that
#	extraction is done a pseudo-basic-block at a time and the address of
#	the scache entry of a branch target is recorded as well.
#	Additional speedups are then possible by defering Ctrl-C checking
#	to the end of basic blocks and by threading the insns together.
#	We call them pseudo-basic-block's instead of just basic-blocks because
#	they're not necessarily basic-blocks, though normally are.
#
# -parallel: cpu can execute multiple instructions parallely
#
#	This option is specified in addition to -simple, -scache, -pbb.
#	Note that while the code can determine if the cpu supports parallel
#	execution with HAVE_PARALLEL_INSNS [and thus this option is
#	technically unnecessary], having this option cuts down on the clutter
#	in the result.
#
# -switch file: specify file containing semantics implemented as a switch()
#
# -cpu <cpu-family>
#
#	Specify the cpu family name.
#
# -infile <input-file>
#
#	Specify the mainloop.in input file.
#
# Only one of -scache/-pbb may be selected.
# -simple is the default.
#
####
#
# TODO
# - build mainloop.in from .cpu file

type=mono
#scache=
#fast=
#full_switch=
#pbb=
#parallel=
switch=
cpu="unknown"
infile=""

while test $# -gt 0
do
	case $1 in
	-mono) type=mono ;;
	-multi) type=multi ;;
	-no-fast) ;;
	-fast) fast=yes ;;
	-full-switch) full_switch=yes ;;
	-simple) ;;
	-scache) scache=yes ;;
	-pbb) pbb=yes ;;
	-no-parallel) ;;
	-parallel) parallel=yes ;;
	-switch) shift ; switch=$1 ;;
	-cpu) shift ; cpu=$1 ;;
	-infile) shift ; infile=$1 ;;
	*) echo "unknown option: $1" >&2 ; exit 1 ;;
	esac
	shift
done

# Argument validation.

if [ x$scache = xyes -a x$pbb = xyes ] ; then
    echo "only one of -scache and -pbb may be selected" >&2
    exit 1
fi

if [ "x$cpu" = xunknown ] ; then
    echo "cpu family not specified" >&2
    exit 1
fi

if [ "x$infile" = x ] ; then
    echo "mainloop.in not specified" >&2
    exit 1
fi

lowercase='abcdefghijklmnopqrstuvwxyz'
uppercase='ABCDEFGHIJKLMNOPQRSTUVWXYZ'
CPU=`echo ${cpu} | tr "${lowercase}" "${uppercase}"`

##########################################################################

rm -f eng.hin
exec 1>eng.hin

echo "/* engine configuration for ${cpu} */"
echo ""

echo "/* WITH_FAST: non-zero if a fast version of the engine is available"
echo "   in addition to the full-featured version.  */"
if [ x$fast = xyes ] ; then
	echo "#define WITH_FAST 1"
else
	echo "#define WITH_FAST 0"
fi

echo ""
echo "/* WITH_SCACHE_PBB_${CPU}: non-zero if the pbb engine was selected.  */"
if [ x$pbb = xyes ] ; then
	echo "#define WITH_SCACHE_PBB_${CPU} 1"
else
	echo "#define WITH_SCACHE_PBB_${CPU} 0"
fi

echo ""
echo "/* HAVE_PARALLEL_INSNS: non-zero if cpu can parallelly execute > 1 insn.  */"
if [ x$parallel = xyes ] ; then
	echo "#define HAVE_PARALLEL_INSNS 1"
else
	echo "#define HAVE_PARALLEL_INSNS 0"
fi

if [ "x$switch" != x ] ; then
	echo ""
	echo "/* WITH_SEM_SWITCH_FULL: non-zero if full-featured engine is"
	echo "   implemented as a switch().  */"
	if [ x$fast != xyes -o x$full_switch = xyes ] ; then
		echo "#define WITH_SEM_SWITCH_FULL 1"
	else
		echo "#define WITH_SEM_SWITCH_FULL 0"
	fi
	echo ""
	echo "/* WITH_SEM_SWITCH_FAST: non-zero if fast engine is"
	echo "   implemented as a switch().  */"
	if [ x$fast = xyes ] ; then
		echo "#define WITH_SEM_SWITCH_FAST 1"
	else
		echo "#define WITH_SEM_SWITCH_FAST 0"
	fi
fi

# Decls of functions we define.

echo ""
echo "/* Functions defined in the generated mainloop.c file"
echo "   (which doesn't necessarily have that file name).  */"
echo ""
echo "extern ENGINE_FN ${cpu}_engine_run_full;"
echo "extern ENGINE_FN ${cpu}_engine_run_fast;"

if [ x$pbb = xyes ] ; then
	echo ""
	echo "extern SEM_PC ${cpu}_pbb_begin (SIM_CPU *, int);"
	echo "extern SEM_PC ${cpu}_pbb_chain (SIM_CPU *, SEM_ARG);"
	echo "extern SEM_PC ${cpu}_pbb_cti_chain (SIM_CPU *, SEM_ARG, SEM_PC *, PCADDR);"
	echo "extern void ${cpu}_pbb_before (SIM_CPU *, SCACHE *);"
	echo "extern void ${cpu}_pbb_after (SIM_CPU *, SCACHE *);"
fi

##########################################################################

rm -f tmp-mloop.cin mloop.cin
exec 1>tmp-mloop.cin

# We use @cpu@ instead of ${cpu} because we still want to run sed to handle
# transformation of @cpu@ for mainloop.in.

cat << EOF
/* This file is generated by the genmloop script.  DO NOT EDIT! */

/* Enable switch() support in cgen headers.  */
#define SEM_IN_SWITCH

#define WANT_CPU
#define WANT_CPU_${CPU}

#include "sim-main.h"
#include "bfd.h"
#include "cgen-mem.h"
#include "cgen-ops.h"
#include "cpu-opc.h"
#include "cpu-sim.h"
#include "sim-assert.h"

EOF

${SHELL} $infile support

##########################################################################

# Simple engine: fetch an instruction, execute the instruction.

if [ x$scache != xyes -a x$pbb != xyes ] ; then

    cat << EOF

#define FAST_P 0

void
${cpu}_engine_run_full (SIM_CPU *current_cpu)
{
#define FAST_P 0
  SIM_DESC current_state = CPU_STATE (current_cpu);
  SCACHE cache[MAX_LIW_INSNS];
  SCACHE *sc = &cache[0];

EOF

if [ x$parallel = xyes ] ; then
  cat << EOF
  PAREXEC pbufs[MAX_PARALLEL_INSNS];
  PAREXEC *par_exec;

EOF
fi

# Any initialization code before looping starts.
# Note that this code may declare some locals.
${SHELL} $infile init

if [ x$parallel = xyes ] ; then
  cat << EOF

#if defined (HAVE_PARALLEL_EXEC) && defined (__GNUC__)
  {
    if (! CPU_IDESC_READ_INIT_P (current_cpu))
      {
/* ??? Later maybe paste read.c in when building mainloop.c.  */
#define DEFINE_LABELS
#include "readx.c"
	CPU_IDESC_READ_INIT_P (current_cpu) = 1;
      }
  }
#endif

EOF
fi

cat << EOF

  do
    {
/* begin full-{extract,exec}-simple */
EOF

${SHELL} $infile extract-simple
echo ""
${SHELL} $infile full-exec-simple

cat << EOF
/* end full-{extract,exec}-simple */

      ++ CPU_INSN_COUNT (current_cpu);
    }
  while (0 /*CPU_RUNNING_P (current_cpu)*/);
#undef FAST_P
}

#undef FAST_P

EOF

####################################

# Simple engine: fast version.
# ??? A somewhat dubious effort, but for completeness' sake.

if [ x$fast = xyes ] ; then

    cat << EOF

#define FAST_P 1

FIXME

#undef FAST_P

EOF

fi # -fast

fi # simple engine

##########################################################################

# Scache engine: lookup insn in scache, fetch if missing, then execute it.

if [ x$scache = xyes ] ; then

    cat << EOF

static INLINE SCACHE *
${cpu}_scache_lookup (SIM_CPU *current_cpu, PCADDR vpc, SCACHE *scache,
                     unsigned int hash_mask, int FAST_P)
{
  /* First step: look up current insn in hash table.  */
  SCACHE *sc = scache + SCACHE_HASH_PC (vpc, hash_mask);

  /* If the entry isn't the one we want (cache miss),
     fetch and decode the instruction.  */
  if (sc->argbuf.addr != vpc)
    {
      insn_t insn;

      if (FAST_P)
	PROFILE_COUNT_SCACHE_MISS (current_cpu);

/* begin extract-scache */
EOF

${SHELL} $infile extract-scache

cat << EOF
/* end extract-scache */
    }
  else if (FAST_P)
    {
      PROFILE_COUNT_SCACHE_HIT (current_cpu);
      /* Make core access statistics come out right.
	 The size is a guess, but it's currently not used either.  */
      PROFILE_COUNT_CORE (current_cpu, vpc, 2, exec_map);
    }

  return sc;
}

#define FAST_P 0

void
${cpu}_engine_run_full (SIM_CPU *current_cpu)
{
  SIM_DESC current_state = CPU_STATE (current_cpu);
  SCACHE *scache = CPU_SCACHE_CACHE (current_cpu);
  unsigned int hash_mask = CPU_SCACHE_HASH_MASK (current_cpu);
  SEM_PC vpc;

EOF

if [ x$parallel = xyes ] ; then
  cat << EOF
  PAREXEC pbufs[MAX_PARALLEL_INSNS];
  PAREXEC *par_exec;

EOF
fi

# Any initialization code before looping starts.
# Note that this code may declare some locals.
${SHELL} $infile init

if [ x$parallel = xyes ] ; then
  cat << EOF

#if defined (HAVE_PARALLEL_EXEC) && defined (__GNUC__)
  {
    if (! CPU_IDESC_READ_INIT_P (current_cpu))
      {
/* ??? Later maybe paste read.c in when building mainloop.c.  */
#define DEFINE_LABELS
#include "readx.c"
	CPU_IDESC_READ_INIT_P (current_cpu) = 1;
      }
  }
#endif

EOF
fi

cat << EOF

  vpc = GET_H_PC ();

  do
    {
      SCACHE *sc;

      sc = ${cpu}_scache_lookup (current_cpu, vpc, scache, hash_mask, FAST_P);

/* begin full-exec-scache */
EOF

${SHELL} $infile full-exec-scache

cat << EOF
/* end full-exec-scache */

      SET_H_PC (vpc);

      ++ CPU_INSN_COUNT (current_cpu);
    }
  while (0 /*CPU_RUNNING_P (current_cpu)*/);
}

#undef FAST_P

EOF

####################################

# Scache engine: fast version.

if [ x$fast = xyes ] ; then

    cat << EOF

#define FAST_P 1

void
${cpu}_engine_run_fast (SIM_CPU *current_cpu)
{
  SIM_DESC current_state = CPU_STATE (current_cpu);
  SCACHE *scache = CPU_SCACHE_CACHE (current_cpu);
  unsigned int hash_mask = CPU_SCACHE_HASH_MASK (current_cpu);
  SEM_PC vpc;

EOF

if [ x$parallel = xyes ] ; then
  cat << EOF
  PAREXEC pbufs[MAX_PARALLEL_INSNS];
  PAREXEC *par_exec;

EOF
fi

# Any initialization code before looping starts.
# Note that this code may declare some locals.
${SHELL} $infile init

if [ x$parallel = xyes ] ; then
  cat << EOF

#if defined (HAVE_PARALLEL_EXEC) && defined (__GNUC__)
  {
    if (! CPU_IDESC_READ_INIT_P (current_cpu))
      {
/* ??? Later maybe paste read.c in when building mainloop.c.  */
#define DEFINE_LABELS
#include "readx.c"
	CPU_IDESC_READ_INIT_P (current_cpu) = 1;
      }
  }
#endif

EOF
fi # parallel = yes

cat << EOF

#if WITH_SEM_SWITCH_FAST && defined (__GNUC__)
  {
    if (! CPU_IDESC_SEM_INIT_P (current_cpu))
      {
/* ??? Later maybe paste sem-switch.c in when building mainloop.c.  */
#define DEFINE_LABELS
#include "$switch"
	CPU_IDESC_SEM_INIT_P (current_cpu) = 1;
      }
  }
#endif

  vpc = GET_H_PC ();

  do
    {
      SCACHE *sc;

      sc = ${cpu}_scache_lookup (current_cpu, vpc, scache, hash_mask, FAST_P);

/* begin fast-exec-scache */
EOF

${SHELL} $infile fast-exec-scache

cat << EOF
/* end fast-exec-scache */

      SET_H_PC (vpc);

      ++ CPU_INSN_COUNT (current_cpu);
    }
  while (0 /*CPU_RUNNING_P (current_cpu)*/);
}

#undef FAST_P

EOF

fi # -fast

fi # -scache

##########################################################################

# Compilation engine: lookup insn in scache, extract a pbb
# (pseudo-basic-block) if missing, then execute the pbb.
# A "pbb" is a sequence of insns up to the next cti insn or until
# some prespecified maximum.
# CTI: control transfer instruction.

if [ x$pbb = xyes ] ; then

    cat << EOF

/* Record address of cti terminating a pbb.  */
#define SET_CTI_VPC(sc) do { cti_sc = (sc); } while (0)
/* Record number of [real] insns in pbb.  */
#define SET_INSN_COUNT(n) do { insn_count = (n); } while (0)

/* Fetch and extract a pseudo-basic-block.
   FAST_P is non-zero if no tracing/profiling/etc. is wanted.  */

INLINE SEM_PC
${cpu}_pbb_begin (SIM_CPU *current_cpu, int FAST_P)
{
  SEM_PC new_vpc;
  PCADDR pc;
  SCACHE *sc;
  int max_insns = CPU_SCACHE_MAX_CHAIN_LENGTH (current_cpu);

  pc = GET_H_PC ();

  new_vpc = scache_lookup_or_alloc (current_cpu, pc, max_insns, &sc);
  if (! new_vpc)
    {
      int insn_count = 0;
      SCACHE *orig_sc = sc;
      SCACHE *cti_sc = NULL;
      int slice_insns = CPU_MAX_SLICE_INSNS (current_cpu);

      /* First figure out how many instructions to compile.
	 MAX_INSNS is the size of the allocated buffer, which includes space
	 for before/after handlers if they're being used.
	 SLICE_INSNS is the maxinum number of real insns that can be
	 executed.  Zero means "as many as we want".  */
      /* ??? max_insns is serving two incompatible roles.
	 1) Number of slots available in scache buffer.
	 2) Number of real insns to execute.
	 They're incompatible because there are virtual insns emitted too
	 (chain,cti-chain,before,after handlers).  */

      if (slice_insns == 1)
	{
	  /* No need to worry about extra slots required for virtual insns
	     and parallel exec support because MAX_CHAIN_LENGTH is
	     guaranteed to be big enough to execute at least 1 insn!  */
	  max_insns = 1;
	}
      else
	{
	  /* Allow enough slop so that while compiling insns, if max_insns > 0
	     then there's guaranteed to be enough space to emit one real insn.
	     MAX_CHAIN_LENGTH is typically much longer than
	     the normal number of insns between cti's anyway.  */
	  max_insns -= (1 /* one for the trailing chain insn */
			+ (FAST_P
			   ? 0
			   : (1 + MAX_PARALLEL_INSNS) /* before+after */)
			+ (MAX_PARALLEL_INSNS > 1
			   ? (MAX_PARALLEL_INSNS * 2)
			   : 0));

	  /* Account for before/after handlers.  */
	  if (! FAST_P)
	    slice_insns *= 3;

	  if (slice_insns > 0
	      && slice_insns < max_insns)
	    max_insns = slice_insns;
	}

      new_vpc = sc;

      /* SC,PC must be updated to point passed the last entry used.
	 SET_CTI_VPC must be called if pbb is terminated by a cti.
	 SET_INSN_COUNT must be called to record number of real insns in
	 pbb [could be computed by us of course, extra cpu but perhaps
	 negligible enough].  */

/* begin extract-pbb */
EOF

${SHELL} $infile extract-pbb

cat << EOF
/* end extract-pbb */

      /* The last one is a pseudo-insn to link to the next chain.
	 It is also used to record the insn count for this chain.  */
      {
	const IDESC *id;

	/* Was pbb terminated by a cti?  */
	if (cti_sc)
	  {
	    id = & CPU_IDESC (current_cpu) [${CPU}_INSN_X_CTI_CHAIN];
	  }
	else
	  {
	    id = & CPU_IDESC (current_cpu) [${CPU}_INSN_X_CHAIN];
	  }
	SEM_SET_CODE (&sc->argbuf, id, FAST_P);
	sc->argbuf.idesc = id;
	sc->argbuf.addr = pc;
	sc->argbuf.fields.chain.insn_count = insn_count;
	sc->argbuf.fields.chain.next = 0;
	++sc;
      }

      /* Update the pointer to the next free entry.  */
      CPU_SCACHE_NEXT_FREE (current_cpu) = sc;
      /* Record length of chain if profiling.
	 This includes virtual insns since they count against
	 max_insns too.  */
      if (! FAST_P)
	PROFILE_COUNT_SCACHE_CHAIN_LENGTH (current_cpu, sc - orig_sc);
    }

  return new_vpc;
}

/* Chain to the next block from a non-cti terminated previous block.  */

INLINE SEM_PC
${cpu}_pbb_chain (SIM_CPU *current_cpu, SEM_ARG sem_arg)
{
  ARGBUF *abuf = SEM_ARGBUF (sem_arg);

  PBB_UPDATE_INSN_COUNT (current_cpu, sem_arg);

  SET_H_PC (abuf->addr);

  /* If not running forever, exit back to main loop.  */
  if (CPU_MAX_SLICE_INSNS (current_cpu) != 0)
    CPU_RUNNING_P (current_cpu) = 0;

  /* If chained to next block, go straight to it.  */
  if (abuf->fields.chain.next)
    return abuf->fields.chain.next;
  /* See if next block has already been compiled.  */
  abuf->fields.chain.next = scache_lookup (current_cpu, abuf->addr);
  if (abuf->fields.chain.next)
    return abuf->fields.chain.next;
  /* Nope, so next insn is a virtual insn to invoke the compiler
     (begin a pbb).  */
  return CPU_SCACHE_PBB_BEGIN (current_cpu);
}

/* Chain to the next block from a cti terminated previous block.
   NEW_VPC_PTR is one of SEM_BRANCH_UNTAKEN, SEM_BRANCH_UNCACHEABLE, or
   a pointer to a location containing the SEM_PC of the branch's address.
   NEW_PC is the target's branch address, and is only valid if
   NEW_VPC_PTR != SEM_BRANCH_UNTAKEN.  */

INLINE SEM_PC
${cpu}_pbb_cti_chain (SIM_CPU *current_cpu, SEM_ARG sem_arg,
		     SEM_PC *new_vpc_ptr, PCADDR new_pc)
{
  ARGBUF *abuf;

  PBB_UPDATE_INSN_COUNT (current_cpu, sem_arg);

  /* If not running forever, exit back to main loop.  */
  if (CPU_MAX_SLICE_INSNS (current_cpu) != 0)
    CPU_RUNNING_P (current_cpu) = 0;

  /* Restart compiler if we branched to an uncacheable address
     (e.g. "j reg").  */
  if (new_vpc_ptr == SEM_BRANCH_UNCACHEABLE)
    {
      SET_H_PC (new_pc);
      return CPU_SCACHE_PBB_BEGIN (current_cpu);
    }

  /* If branch wasn't taken, update the pc and set BR_ADDR_PTR to our
     next chain ptr.  */
  if (new_vpc_ptr == SEM_BRANCH_UNTAKEN)
    {
      abuf = SEM_ARGBUF (sem_arg);
      SET_H_PC (abuf->addr);
      new_vpc_ptr = &abuf->fields.chain.next;
    }
  else
    {
      SET_H_PC (new_pc);
    }

  /* If chained to next block, go straight to it.  */
  if (*new_vpc_ptr)
    return *new_vpc_ptr;
  /* See if next block has already been compiled.  */
  *new_vpc_ptr = scache_lookup (current_cpu, GET_H_PC ());
  if (*new_vpc_ptr)
    return *new_vpc_ptr;
  /* Nope, so next insn is a virtual insn to invoke the compiler
     (begin a pbb).  */
  return CPU_SCACHE_PBB_BEGIN (current_cpu);
}

/* x-before handler.
   This is called before each insn.  */

void
${cpu}_pbb_before (SIM_CPU *current_cpu, SCACHE *sc)
{
  SEM_ARG sem_arg = sc;
  const ARGBUF *abuf = SEM_ARGBUF (sem_arg);
  int first_p = abuf->fields.before.first_p;
  const ARGBUF *cur_abuf = SEM_ARGBUF (sc + 1);
  const IDESC *cur_idesc = cur_abuf->idesc;
  PCADDR pc = cur_abuf->addr;

  PROFILE_COUNT_INSN (current_cpu, pc, cur_idesc->num);

  /* If this isn't the first insn, finish up the previous one.  */

  if (! first_p)
    {
      if (PROFILE_MODEL_P (current_cpu))
	{
	  const SEM_ARG prev_sem_arg = sc - 1;
	  const ARGBUF *prev_abuf = SEM_ARGBUF (prev_sem_arg);
	  const IDESC *prev_idesc = prev_abuf->idesc;
	  int cycles;

	  cycles = (*prev_idesc->timing->model_fn) (current_cpu, prev_sem_arg);
	  ${cpu}_model_insn_after (current_cpu, 0 /*last_p*/, cycles);
	}

      TRACE_INSN_FINI (current_cpu, 0 /*last_p*/);
    }

  /* FIXME: Later make cover macros: PROFILE_INSN_{INIT,FINI}.  */
  if (PROFILE_MODEL_P (current_cpu))
    ${cpu}_model_insn_before (current_cpu, first_p);

  TRACE_INSN_INIT (current_cpu, first_p);
  TRACE_INSN (current_cpu, cur_idesc->opcode, cur_abuf, cur_abuf->addr);
}

/* x-after handler.
   This is called after a serial insn or at the end of a group of parallel
   insns.  */

void
${cpu}_pbb_after (SIM_CPU *current_cpu, SCACHE *sc)
{
  SEM_ARG sem_arg = sc;
  const ARGBUF *abuf = SEM_ARGBUF (sem_arg);

  if (PROFILE_MODEL_P (current_cpu))
    {
      const SEM_ARG prev_sem_arg = sc - 1;
      const ARGBUF *prev_abuf = SEM_ARGBUF (prev_sem_arg);
      const IDESC *prev_idesc = prev_abuf->idesc;
      int cycles;

      cycles = (*prev_idesc->timing->model_fn) (current_cpu, prev_sem_arg);
      ${cpu}_model_insn_after (current_cpu, 1 /*last_p*/, cycles);
    }
  TRACE_INSN_FINI (current_cpu, 1 /*last_p*/);
}

#define FAST_P 0

void
${cpu}_engine_run_full (SIM_CPU *current_cpu)
{
  SIM_DESC current_state = CPU_STATE (current_cpu);
  SCACHE *scache = CPU_SCACHE_CACHE (current_cpu);
  /* virtual program counter */
  SEM_PC vpc;
#if WITH_SEM_SWITCH_FULL
  /* For communication between cti's and cti-chain.  */
  PCADDR pbb_br_npc;
  SEM_PC *pbb_br_npc_ptr;
#endif

EOF

if [ x$parallel = xyes ] ; then
  cat << EOF
  PAREXEC pbufs[MAX_PARALLEL_INSNS];
  PAREXEC *par_exec = &pbufs[0];

EOF
fi

# Any initialization code before looping starts.
# Note that this code may declare some locals.
${SHELL} $infile init

cat << EOF

  if (! CPU_IDESC_SEM_INIT_P (current_cpu))
    {
      /* ??? 'twould be nice to move this up a level and only call it once.
	 On the other hand, in the "let's go fast" case the test is only done
	 once per pbb (since we only return to the main loop at the end of
	 a pbb).  And in the "let's run until we're done" case we don't return
	 until the program exits.  */

#if WITH_SEM_SWITCH_FULL && defined (__GNUC__)
/* ??? Later maybe paste sem-switch.c in when building mainloop.c.  */
#define DEFINE_LABELS
#include "$switch"
#endif

      /* Initialize the "begin (compile) a pbb" virtual insn.  */
      vpc = CPU_SCACHE_PBB_BEGIN (current_cpu);
      SEM_SET_FULL_CODE (SEM_ARGBUF (vpc),
			 & CPU_IDESC (current_cpu) [${CPU}_INSN_X_BEGIN]);
      vpc->argbuf.idesc = & CPU_IDESC (current_cpu) [${CPU}_INSN_X_BEGIN];

      CPU_IDESC_SEM_INIT_P (current_cpu) = 1;
    }

  CPU_RUNNING_P (current_cpu) = 1;
  /* ??? In the case where we're returning to the main loop after every
     pbb we don't want to call pbb_begin each time (which hashes on the pc
     and does a table lookup).  A way to speed this up is to save vpc
     between calls.  */
  vpc = ${cpu}_pbb_begin (current_cpu, FAST_P);

  do
    {
/* begin full-exec-pbb */
EOF

${SHELL} $infile full-exec-pbb

cat << EOF
/* end full-exec-pbb */
    }
  while (CPU_RUNNING_P (current_cpu));
}

#undef FAST_P

EOF

####################################

# Compile engine: fast version.

if [ x$fast = xyes ] ; then

    cat << EOF

#define FAST_P 1

void
${cpu}_engine_run_fast (SIM_CPU *current_cpu)
{
  SIM_DESC current_state = CPU_STATE (current_cpu);
  SCACHE *scache = CPU_SCACHE_CACHE (current_cpu);
  /* virtual program counter */
  SEM_PC vpc;
#if WITH_SEM_SWITCH_FAST
  /* For communication between cti's and cti-chain.  */
  PCADDR pbb_br_npc;
  SEM_PC *pbb_br_npc_ptr;
#endif

EOF

if [ x$parallel = xyes ] ; then
  cat << EOF
  PAREXEC pbufs[MAX_PARALLEL_INSNS];
  PAREXEC *par_exec = &pbufs[0];

EOF
fi

# Any initialization code before looping starts.
# Note that this code may declare some locals.
${SHELL} $infile init

cat << EOF

  if (! CPU_IDESC_SEM_INIT_P (current_cpu))
    {
      /* ??? 'twould be nice to move this up a level and only call it once.
	 On the other hand, in the "let's go fast" case the test is only done
	 once per pbb (since we only return to the main loop at the end of
	 a pbb).  And in the "let's run until we're done" case we don't return
	 until the program exits.  */

#if WITH_SEM_SWITCH_FAST && defined (__GNUC__)
/* ??? Later maybe paste sem-switch.c in when building mainloop.c.  */
#define DEFINE_LABELS
#include "$switch"
#endif

      /* Initialize the "begin (compile) a pbb" virtual insn.  */
      vpc = CPU_SCACHE_PBB_BEGIN (current_cpu);
      SEM_SET_FAST_CODE (SEM_ARGBUF (vpc),
			 & CPU_IDESC (current_cpu) [${CPU}_INSN_X_BEGIN]);
      vpc->argbuf.idesc = & CPU_IDESC (current_cpu) [${CPU}_INSN_X_BEGIN];

      CPU_IDESC_SEM_INIT_P (current_cpu) = 1;
    }

  CPU_RUNNING_P (current_cpu) = 1;
  /* ??? In the case where we're returning to the main loop after every
     pbb we don't want to call pbb_begin each time (which hashes on the pc
     and does a table lookup).  A way to speed this up is to save vpc
     between calls.  */
  vpc = ${cpu}_pbb_begin (current_cpu, FAST_P);

  do
    {
/* begin fast-exec-pbb */
EOF

${SHELL} $infile fast-exec-pbb

cat << EOF
/* end fast-exec-pbb */
    }
  while (CPU_RUNNING_P (current_cpu));
}

#undef FAST_P

EOF
fi # -fast

fi # -pbb

sed -e "s/@cpu@/$cpu/g" -e "s/@CPU@/$CPU/g" < tmp-mloop.cin > mloop.cin
rc=$?
rm -f tmp-mloop.cin

exit $rc
