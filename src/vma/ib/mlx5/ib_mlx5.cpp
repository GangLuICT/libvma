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

#if defined(DEFINED_DIRECT_VERBS)

#include "vma/util/utils.h"
#include "vma/ib/mlx5/ib_mlx5.h"


int vma_ib_mlx5_get_qp(struct ibv_qp *qp, vma_ib_mlx5_qp_t *mlx5_qp)
{
    int ret = 0;
    vma_ib_mlx5dv_t obj;
    vma_ib_mlx5dv_qp_t dqp;

    memset(&obj, 0, sizeof(obj));
    memset(&dqp, 0, sizeof(dqp));

    obj.dv.qp.in = qp;
    obj.dv.qp.out = &dqp.dv;
    ret = vma_ib_mlx5dv_init_obj(&obj, MLX5DV_OBJ_QP);
    if (ret != 0) {
        return ret;
    }

	mlx5_qp->qpn        = qp->qp_num;
	mlx5_qp->sq.dbrec   = &dqp.dv.dbrec[MLX5_SND_DBR];
	mlx5_qp->sq.buf     = dqp.dv.sq.buf;
	mlx5_qp->sq.wqe_cnt = dqp.dv.sq.wqe_cnt;
	mlx5_qp->sq.stride  = dqp.dv.sq.stride;
	mlx5_qp->rq.dbrec   = &dqp.dv.dbrec[MLX5_RCV_DBR];
	mlx5_qp->rq.buf     = dqp.dv.rq.buf;
	mlx5_qp->rq.wqe_cnt = dqp.dv.rq.wqe_cnt;
	mlx5_qp->rq.stride  = dqp.dv.rq.stride;
	mlx5_qp->rq.head    = vma_ib_mlx5_get_rq_head(qp);
	mlx5_qp->rq.tail    = vma_ib_mlx5_get_rq_tail(qp);
	mlx5_qp->bf.reg     = dqp.dv.bf.reg;
	mlx5_qp->bf.size    = dqp.dv.bf.size;

    return 0;
}

int vma_ib_mlx5_get_cq(struct ibv_cq *cq, vma_ib_mlx5_cq_t *mlx5_cq)
{
    int ret = 0;
    vma_ib_mlx5dv_t obj;
    vma_ib_mlx5dv_cq_t dcq;

    memset(&obj, 0, sizeof(obj));
    memset(&dcq, 0, sizeof(dcq));

    obj.dv.cq.in = cq;
    obj.dv.cq.out = &dcq.dv;
    ret = vma_ib_mlx5dv_init_obj(&obj, MLX5DV_OBJ_CQ);
    if (ret != 0) {
        return ret;
    }

    mlx5_cq->cq_num       = dcq.dv.cqn;
    mlx5_cq->cq_ci        = 0;
    mlx5_cq->cqe_count    = dcq.dv.cqe_cnt;
    mlx5_cq->cqe_size     = dcq.dv.cqe_size;
    mlx5_cq->cqe_size_log = ilog_2(dcq.dv.cqe_size);
    mlx5_cq->dbrec        = dcq.dv.dbrec;

    /* Move buffer forward for 128b CQE, so we would get pointer to the 2nd
     * 64b when polling.
     */
    mlx5_cq->cq_buf       = (uint8_t *)dcq.dv.buf + dcq.dv.cqe_size - sizeof(struct mlx5_cqe64);

    return 0;
}
#endif /* DEFINED_DIRECT_VERBS */
