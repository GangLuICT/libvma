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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(DEFINED_DIRECT_VERBS) && (DEFINED_DIRECT_VERBS == 2)

#include "vma/ib/mlx5/ib_mlx5.h"

static int vma_ib_mlx5dv_get_qp(struct ibv_qp *qp, vma_ib_mlx5dv_qp_t *mlx5_qp);
static int vma_ib_mlx5dv_get_cq(struct ibv_cq *cq, vma_ib_mlx5dv_cq_t *mlx5_cq);
static int vma_ib_mlx5dv_get_rwq(struct ibv_exp_wq *wq, vma_ib_mlx5dv_rwq_t *mlx5_rwq);


int vma_ib_mlx5dv_init_obj(vma_ib_mlx5dv_t *obj, uint64_t obj_type)
{
	int ret = 0;

	if (obj_type & MLX5DV_OBJ_QP) {
		ret = vma_ib_mlx5dv_get_qp(obj->dv.qp.in,
				container_of(obj->dv.qp.out, vma_ib_mlx5dv_qp_t, dv));
	}

	if (!ret && (obj_type & MLX5DV_OBJ_CQ)) {
		ret = vma_ib_mlx5dv_get_cq(obj->dv.cq.in,
				container_of(obj->dv.cq.out, vma_ib_mlx5dv_cq_t, dv));
	}

	if (!ret && (obj_type & MLX5DV_OBJ_RWQ)) {
		ret = vma_ib_mlx5dv_get_rwq(obj->dv.rwq.in,
				container_of(obj->dv.rwq.out, vma_ib_mlx5dv_rwq_t, dv));
	}

	return ret;
}

static int vma_ib_mlx5dv_get_qp(struct ibv_qp *qp, vma_ib_mlx5dv_qp_t *mlx5_qp)
{
	int ret = 0;

#if HAVE_DECL_IBV_MLX5_EXP_GET_QP_INFO
	struct ibv_mlx5_qp_info ibv_qp_info;
	struct mlx5_qp *mqp = to_mqp(qp);

	ret = ibv_mlx5_exp_get_qp_info(qp, &ibv_qp_info);
	if (ret != 0) {
		return ret;
	}

	mlx5_qp->dv.dbrec      = ibv_qp_info.dbrec;
	mlx5_qp->dv.sq.buf     = (mqp->sq_buf_size ?
			(void *)((uintptr_t)mqp->sq_buf.buf) : /* IBV_QPT_RAW_PACKET or Underly QP */
			(void *)((uintptr_t)mqp->buf.buf + mqp->sq.offset));
	mlx5_qp->dv.sq.wqe_cnt = ibv_qp_info.sq.wqe_cnt;
	mlx5_qp->dv.sq.stride  = ibv_qp_info.sq.stride;
	mlx5_qp->dv.rq.buf     = ibv_qp_info.rq.buf;
	mlx5_qp->dv.rq.wqe_cnt = ibv_qp_info.rq.wqe_cnt;
	mlx5_qp->dv.rq.stride  = ibv_qp_info.rq.stride;
	mlx5_qp->dv.bf.reg     = ibv_qp_info.bf.reg;
	mlx5_qp->dv.bf.size    = ibv_qp_info.bf.size;

#else
	struct mlx5_qp *mqp = to_mqp(qp);

	if ((mqp->sq.cur_post != 0) || (mqp->rq.head != 0)) {
		return -ENODEV;
	}

	mlx5_qp->dv.dbrec      = mqp->gen_data.db;
	mlx5_qp->dv.sq.buf     = (mqp->sq_buf_size ?
			(void *)((uintptr_t)mqp->sq_buf.buf) : /* IBV_QPT_RAW_PACKET or Underly QP */
			(void *)((uintptr_t)mqp->buf.buf + mqp->sq.offset));
	mlx5_qp->dv.sq.wqe_cnt = mqp->sq.wqe_cnt;
	mlx5_qp->dv.sq.stride  = 1 << mqp->sq.wqe_shift;
	mlx5_qp->dv.rq.buf     = (void *)((uintptr_t)mqp->buf.buf + mqp->rq.offset);
	mlx5_qp->dv.rq.wqe_cnt = mqp->rq.wqe_cnt;
	mlx5_qp->dv.rq.stride  = 1 << mqp->rq.wqe_shift;
	mlx5_qp->dv.bf.reg     = mqp->gen_data.bf->reg;
	mlx5_qp->dv.bf.size    = (mqp->gen_data.bf->uuarn > 0 ?
			mqp->gen_data.bf->buf_size : 0);

#endif /* HAVE_DECL_IBV_MLX5_EXP_GET_QP_INFO */

	return ret;
}

static int vma_ib_mlx5dv_get_cq(struct ibv_cq *cq, vma_ib_mlx5dv_cq_t *mlx5_cq)
{
	int ret = 0;

#if HAVE_DECL_IBV_MLX5_EXP_GET_CQ_INFO
	struct ibv_mlx5_cq_info ibv_cq_info;

	ret = ibv_mlx5_exp_get_cq_info(cq, &ibv_cq_info);
	if (ret != 0) {
		return ret;
	}

	mlx5_cq->dv.buf      = ibv_cq_info.buf;
	mlx5_cq->dv.dbrec    = ibv_cq_info.dbrec;
	mlx5_cq->dv.cqe_cnt  = ibv_cq_info.cqe_cnt;
	mlx5_cq->dv.cqe_size = ibv_cq_info.cqe_size;
	mlx5_cq->dv.cqn      = ibv_cq_info.cqn;

#else
    struct mlx5_cq *mcq = to_mcq(cq);

    if (mcq->cons_index != 0) {
        return -ENODEV;
    }

    mlx5_cq->dv.buf      = mcq->active_buf->buf;
	mlx5_cq->dv.dbrec    = mcq->dbrec;
    mlx5_cq->dv.cqe_cnt  = mcq->ibv_cq.cqe + 1;
    mlx5_cq->dv.cqe_size = mcq->cqe_sz;
    mlx5_cq->dv.cqn      = mcq->cqn;

#endif /* HAVE_DECL_IBV_MLX5_EXP_GET_CQ_INFO */

	return ret;
}

static int vma_ib_mlx5dv_get_rwq(struct ibv_exp_wq *wq, vma_ib_mlx5dv_rwq_t *mlx5_rwq)
{
	int ret = 0;

	struct mlx5_rwq *rwq = container_of(wq, struct mlx5_rwq, wq);

	mlx5_rwq->dv.buf     = (uint8_t *)rwq->buf.buf + rwq->rq.offset;
	mlx5_rwq->dv.dbrec   = rwq->db;
	mlx5_rwq->dv.wqe_cnt = rwq->rq.wqe_cnt;
	mlx5_rwq->dv.stride  = (1 << rwq->rq.wqe_shift);

	return ret;
}

void vma_ib_mlx5_update_cq_ci(struct ibv_cq *cq, unsigned cq_ci)
{
	struct mlx5_cq *mcq = to_mcq(cq);

	mcq->cons_index = cq_ci;
}

#endif /* (DEFINED_DIRECT_VERBS == 2) */
