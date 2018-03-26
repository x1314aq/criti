//
// Created by xaq on 11/15/17.
//

#include "neutron_transport.h"
#include "acedata.h"
#include "criticality.h"


static int fis_MT[5] = {18, 19, 20, 21, 38};

void
treat_fission(particle_status_t *par_status,
              RNG_t *RNG,
              double *keff_wgt_sum,
              bank_t *bank,
              int *bank_cnt,
              double keff_final)
{
    nuclide_t *nuc;
    nuc_xs_t *cur_nuc_xs;
    double fis_sub_cs[5];
    bank_t *cur_fis_bank;
    double fis_R;
    int i, prev_bank_cnt;

    nuc = par_status->nuc;
    cur_nuc_xs = par_status->nuc_xs;
    prev_bank_cnt = *bank_cnt;
    cur_fis_bank = &bank[prev_bank_cnt];

    if(cur_nuc_xs->fis <= ZERO) return;

    if(nuc->LSIG[18] > 0)  /* 总裂变截面MT=18存在 */
        fis_sub_cs[0] = get_nuc_mt_cs(nuc, 18, par_status->interp_N0, par_status->interp_K0);
    else
        for(i = 1; i < 5; i++)
            fis_sub_cs[i] = get_nuc_mt_cs(nuc, fis_MT[i], par_status->interp_N0, par_status->interp_K0);

    if(cur_nuc_xs->ptable) {
        double temp = nuc->fis_XSS[par_status->interp_N0] + par_status->interp_K0 * (nuc->fis_XSS[par_status->interp_N0 + 1] - nuc->fis_XSS[par_status->interp_N0]);
        if(!EQ_ZERO(temp)){
            double ff = cur_nuc_xs->fis / temp;
            for(i = 0; i < 5; i++)
                fis_sub_cs[i] *= ff;
        }
    }

    keff_wgt_sum[1] += par_status->wgt * cur_nuc_xs->nu * cur_nuc_xs->fis / cur_nuc_xs->tot;

    if(nuc->LSIG[18] > 0) {
        if(fis_sub_cs[0] > 0) {
            fis_R = par_status->wgt * cur_nuc_xs->nu * fis_sub_cs[0] / cur_nuc_xs->tot / keff_final;
            prev_bank_cnt += get_fis_neu_state(par_status, cur_fis_bank, RNG, fis_MT[0], fis_R, cur_nuc_xs->nu);
        }
    } else {
        for(i = 1; i < 5; i++) {
            if(fis_sub_cs[i] > 0) {
                fis_R = par_status->wgt * cur_nuc_xs->nu * fis_sub_cs[i] / cur_nuc_xs->tot / keff_final;
                prev_bank_cnt += get_fis_neu_state(par_status, cur_fis_bank, RNG, fis_MT[i], fis_R, cur_nuc_xs->nu);
            }
        }
    }
    *bank_cnt = prev_bank_cnt;
}