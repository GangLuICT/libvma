/*
 * Copyright (c) 2001-2018 Mellanox Technologies, Ltd. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef SRC_VMA_IB_MLX5_DV_H_
#define SRC_VMA_IB_MLX5_DV_H_

#ifndef SRC_VMA_IB_MLX5_H_
#error "Use <vma/ib/mlx5/ib_mlx5.h> instead."
#endif

#if defined(DEFINED_DIRECT_VERBS) && (DEFINED_DIRECT_VERBS == 3)

#ifndef offsetof
#define offsetof(type, member) ((uintptr_t) &((type *)0)->member)
#endif

#ifndef container_of
#define container_of(ptr, type, member) (type *)((char *)(ptr) - offsetof(type,member))
#endif

struct mlx5_list_node
{
	struct mlx5_list_node *next, *prev;
};

struct verbs_xrcd {
	struct ibv_xrcd		xrcd;
	uint32_t		comp_mask;
	uint32_t		handle;
};

struct verbs_qp {
	struct ibv_qp		qp;
	uint32_t		comp_mask;
	struct verbs_xrcd       *xrcd;
};

enum mlx5_rsc_type {
	MLX5_RSC_TYPE_QP,
	MLX5_RSC_TYPE_XSRQ,
	MLX5_RSC_TYPE_SRQ,
	MLX5_RSC_TYPE_RWQ,
	MLX5_RSC_TYPE_INVAL,
};

struct mlx5_resource {
	enum mlx5_rsc_type	type;
	uint32_t		rsn;
};

struct mlx5_bitmap {
	uint32_t		last;
	uint32_t		top;
	uint32_t		max;
	uint32_t		avail;
	uint32_t		mask;
	unsigned long	       *table;
};

struct mlx5_hugetlb_mem {
	int			shmid;
	void		       *shmaddr;
	struct mlx5_bitmap	bitmap;
	struct mlx5_list_node	entry;
};

enum mlx5_alloc_type {
	MLX5_ALLOC_TYPE_ANON,
	MLX5_ALLOC_TYPE_HUGE,
	MLX5_ALLOC_TYPE_CONTIG,
	MLX5_ALLOC_TYPE_PREFER_HUGE,
	MLX5_ALLOC_TYPE_PREFER_CONTIG,
	MLX5_ALLOC_TYPE_EXTERNAL,
	MLX5_ALLOC_TYPE_ALL
};

struct mlx5_buf {
	void			       *buf;
	size_t				length;
	int                             base;
	struct mlx5_hugetlb_mem	       *hmem;
	enum mlx5_alloc_type		type;
};

struct mlx5_spinlock {
	pthread_spinlock_t		lock;
	int				in_use;
	int				need_lock;
};

struct mlx5_bf {
	void			       *reg;
	int				need_lock;
	struct mlx5_spinlock		lock;
	unsigned			offset;
	unsigned			buf_size;
	unsigned			uuarn;
	off_t				uar_mmap_offset;
	/* The virtual address of the mmaped uar, applicable for the dynamic use case */
	void				*uar;
	/* Index in the dynamic bfregs portion */
	uint32_t			bfreg_dyn_index;
};

struct mlx5_wq {
	uint64_t		       *wrid;
	unsigned		       *wqe_head;
	struct mlx5_spinlock		lock;
	unsigned			wqe_cnt;
	unsigned			max_post;
	unsigned			head;
	unsigned			tail;
	unsigned			cur_post;
	int				max_gs;
	int				wqe_shift;
	int				offset;
	void			       *qend;
	uint32_t			*wr_data;
};

struct mlx5_qp {
	struct mlx5_resource            rsc; /* This struct must be first */
	struct verbs_qp			verbs_qp;
	struct ibv_qp		       *ibv_qp;
	struct mlx5_buf                 buf;
	void				*sq_start;
	int                             max_inline_data;
	int                             buf_size;
	/* For Raw Packet QP, use different buffers for the SQ and RQ */
	struct mlx5_buf                 sq_buf;
	int				sq_buf_size;
	struct mlx5_bf		       *bf;

	uint8_t				fm_cache;
	uint8_t	                        sq_signal_bits;
	struct mlx5_wq                  sq;

	__be32                         *db;
	struct mlx5_wq                  rq;
	int                             wq_sig;
	uint32_t			qp_cap_cache;
	int				atomics_enabled;
	uint32_t			max_tso;
	uint16_t			max_tso_header;
	int                             rss_qp;
	uint32_t			flags; /* Use enum mlx5_qp_flags */
	enum mlx5dv_dc_type		dc_type;
};


static inline struct mlx5_qp *to_mqp(struct ibv_qp *ibqp)
{
	struct verbs_qp *vqp = (struct verbs_qp *)ibqp;

	return container_of(vqp, struct mlx5_qp, verbs_qp);
}

#endif /* (DEFINED_DIRECT_VERBS == 3) */

#endif /* SRC_VMA_IB_MLX5_DV_H_ */
