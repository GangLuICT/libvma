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

#ifndef SRC_VMA_IB_MLX5_H_
#define SRC_VMA_IB_MLX5_H_

#include <infiniband/verbs.h>

#if defined(DEFINED_DIRECT_VERBS)
#if (DEFINED_DIRECT_VERBS == 2)
#include <infiniband/mlx5_hw.h>
#include "vma/ib/mlx5/ib_mlx5_hw.h"
#elif (DEFINED_DIRECT_VERBS == 3)
#include <infiniband/mlx5dv.h>
#include "vma/ib/mlx5/ib_mlx5_dv.h"
#else
#error "Unsupported Direct VERBS parameter"
#endif
#endif /* DEFINED_DIRECT_VERBS */

#ifndef offsetof
#define offsetof(_type, _member) ((uintptr_t) &((_type *)0)->_member)
#endif

#ifndef container_of
#define container_of(_ptr, _type, _member) (_type *)((char *)(_ptr) - offsetof(_type, _member))
#endif


typedef struct {
	struct mlx5dv_obj dv;
} vma_ib_mlx5dv_t;

typedef struct {
	struct mlx5dv_qp dv;
} vma_ib_mlx5dv_qp_t;

typedef struct {
	struct mlx5dv_cq dv;
} vma_ib_mlx5dv_cq_t;

typedef struct {
	struct mlx5dv_rwq dv;
} vma_ib_mlx5dv_rwq_t;

/**
 * Get internal verbs information.
 */
int vma_ib_mlx5dv_init_obj(vma_ib_mlx5dv_t *obj, uint64_t type);

/* Queue pair */
typedef struct vma_ib_mlx5_qp {
	uint32_t qpn;
	struct {
		volatile uint32_t *dbrec;
		void *buf;
		uint32_t wqe_cnt;
		uint32_t stride;
	} sq;
	struct {
		volatile uint32_t *dbrec;
		void *buf;
		uint32_t wqe_cnt;
		uint32_t stride;
		uint32_t head;
		uint32_t tail;
	} rq;
	struct {
		void *reg;
		uint32_t size;
	} bf;
} vma_ib_mlx5_qp_t;

/* Completion queue */
typedef struct vma_ib_mlx5_cq {
    void               *cq_buf;
    unsigned           cq_num;
    unsigned           cq_ci;
    unsigned           cqe_count;
    unsigned           cqe_size;
    unsigned           cqe_size_log;
    volatile uint32_t  *dbrec;
} vma_ib_mlx5_cq_t;

int vma_ib_mlx5_get_qp(struct ibv_qp *qp, vma_ib_mlx5_qp_t *mlx5_qp);
int vma_ib_mlx5_get_cq(struct ibv_cq *cq, vma_ib_mlx5_cq_t *mlx5_cq);
void vma_ib_mlx5_update_cq_ci(struct ibv_cq *cq, unsigned cq_ci);

#endif /* SRC_VMA_IB_MLX5_H_ */
