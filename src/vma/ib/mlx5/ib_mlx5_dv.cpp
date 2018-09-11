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

#if defined(DEFINED_DIRECT_VERBS) && (DEFINED_DIRECT_VERBS == 3)

#include "vma/ib/mlx5/ib_mlx5.h"

int vma_ib_mlx5dv_init_obj(vma_ib_mlx5dv_t *obj, uint64_t type)
{
	int ret = 0;

	ret = mlx5dv_init_obj(&obj->dv, type);

	return ret;
}

unsigned* vma_ib_mlx5_get_rq_head(struct ibv_qp *qp)
{
	struct mlx5_qp *mqp = to_mqp(qp);

	return &mqp->rq.head;
}

unsigned* vma_ib_mlx5_get_rq_tail(struct ibv_qp *qp)
{
	struct mlx5_qp *mqp = to_mqp(qp);

	return &mqp->rq.tail;
}

void vma_ib_mlx5_update_cq_ci(struct ibv_cq *cq, unsigned cq_ci)
{
	cq = cq;
	cq_ci = cq_ci;
}

#endif /* (DEFINED_DIRECT_VERBS == 3) */
