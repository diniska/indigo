#ifndef PG_BINGO_CONTEXT_H__
#define	PG_BINGO_CONTEXT_H__

typedef struct BingoMetaPageData {
   int bingo_index_version;
   int n_molecules;
   int n_blocks_for_map;
   int n_blocks_for_fp;
   int n_blocks_for_dictionary;
   int offset_dictionary;
   int n_sections;
   int n_pages;
   int index_type;
} BingoMetaPageData;

typedef BingoMetaPageData *BingoMetaPage;

#define BingoPageGetMeta(page) \
	((BingoMetaPage) PageGetContents(page))

typedef struct BingoAutoVacOpts {
   bool enabled;
   int vacuum_threshold;
   int analyze_threshold;
   int vacuum_cost_delay;
   int vacuum_cost_limit;
   int freeze_min_age;
   int freeze_max_age;
   int freeze_table_age;
   double vacuum_scale_factor;
   double analyze_scale_factor;
} BingoAutoVacOpts;

typedef struct BingoIndexOptions {
   int treat_x_as_pseudoatom;
   int ignore_closing_bond_direction_mismatch;
   int fp_ord_size;
   int fp_any_size;
   int fp_tau_size;
   int fp_sim_size;
   int sub_screening_max_bits;
   int sim_screening_pass_mark;
} BingoIndexOptions;


typedef struct BingoStdRdOptions {
   int vl_len_; /* varlena header (do not touch directly!) */
   int fillfactor; /* page fill factor in percent (0..100) */
   BingoAutoVacOpts autovacuum; /* autovacuum-related options */
   BingoIndexOptions index_parameters;
} BingoStdRdOptions;

typedef struct BingoSectionInfoData {
   int n_structures;
   int n_blocks_for_map;
   int n_blocks_for_fp;
   int n_blocks_for_bin;
   int section_size;
   int last_cmf;
   int last_xyz;
   char has_removed;
} BingoSectionInfoData;

typedef struct BingoBlockIdData {
   unsigned short bi_hi;
   unsigned short bi_lo;
} BingoBlockIdData;

typedef struct BingoItemData {
   BingoBlockIdData ip_blkid;
   unsigned short ip_posid;
} BingoItemData;


#endif	/* BINGO_PG_CONTEXT_H */

