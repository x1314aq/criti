//
// Created by xaq on 10/26/17.
//

#include "criticality.h"
#include "sample_method.h"


extern criti_t base_criti;
extern RNG_t base_RNG;
extern double base_start_wgt;
extern nuc_xs_t **base_nuc_xs;
extern int base_num_threads;

void
init_fission_source(pth_arg_t *pth_args)
{
    int i, j;
    int quotient, remainder;

    printf("Initiating fission source...");

    base_criti.tot_start_wgt = ONE * base_criti.cycle_neu_num;

    quotient = base_criti.cycle_neu_num / base_num_threads;
    remainder = base_criti.cycle_neu_num - quotient * base_num_threads;
    for(i = 0; i < base_num_threads; i++){
        pth_args[i].src_cnt = quotient;
        pth_args[i].bank_cnt = 0;
        pth_args[i].nuc_xs = base_nuc_xs[i];
        pth_args[i].col_cnt = 0;
        pth_args[i].keff_final = base_criti.keff_final;
        for(j = 0; j < 3; j++)
            pth_args[i].keff_wgt_sum[j] = ZERO;
    }
    if(remainder)
        for(i = 0; i < remainder; i++)
            pth_args[i].src_cnt++;

    for(i = 0; i < base_num_threads; i++) {
        int sz = 5 * pth_args[i].src_cnt;
        if(sz < 100) sz = 100;
        pth_args[i].src = malloc(sz * sizeof(bank_t));
        pth_args[i].bank = malloc(sz * sizeof(bank_t));
    }

    double ksi1, ksi2, ksi3;
    bank_t *ksrc;
    int ksrc_cnt = 0;
    switch(base_criti.ksrc_type) {
        case POINT: {
            for(i = 0; i < base_num_threads; i++) {
                ksrc_cnt = pth_args[i].src_cnt;
                for(j = 0; j < ksrc_cnt; j++) {
                    get_rand_seed_host(&base_RNG);
                    ksrc = &pth_args[i].src[j];
                    ksrc->pos[0] = base_criti.ksrc_para[0];
                    ksrc->pos[1] = base_criti.ksrc_para[1];
                    ksrc->pos[2] = base_criti.ksrc_para[2];
                }
            }
            break;
        }
        case SLAB: {
            double len_x = base_criti.ksrc_para[1] - base_criti.ksrc_para[0];
            double len_y = base_criti.ksrc_para[3] - base_criti.ksrc_para[2];
            double len_z = base_criti.ksrc_para[5] - base_criti.ksrc_para[4];

            for(i = 0; i < base_num_threads; i++) {
                ksrc_cnt = pth_args[i].src_cnt;
                for(j = 0; j < ksrc_cnt; j++) {
                    get_rand_seed_host(&base_RNG);
                    ksrc = &pth_args[i].src[j];
                    ksrc->pos[0] = base_criti.ksrc_para[0] + get_rand_host(&base_RNG) * len_x;
                    ksrc->pos[1] = base_criti.ksrc_para[1] + get_rand_host(&base_RNG) * len_y;
                    ksrc->pos[2] = base_criti.ksrc_para[2] + get_rand_host(&base_RNG) * len_z;
                }
            }
            break;
        }
        case SPHERE: {
            for(i = 0; i < base_num_threads; i++) {
                ksrc_cnt = pth_args[i].src_cnt;
                for(j = 0; j < ksrc_cnt; j++) {
                    get_rand_seed_host(&base_RNG);
                    ksrc = &pth_args[i].src[j];
                    do {
                        ksi1 = TWO * get_rand_host(&base_RNG) - ONE;
                        ksi2 = TWO * get_rand_host(&base_RNG) - ONE;
                        ksi3 = TWO * get_rand_host(&base_RNG) - ONE;
                    } while(SQUARE(ksi1) + SQUARE(ksi2) + SQUARE(ksi3) > 1);
                    ksrc->pos[0] = base_criti.ksrc_para[0] + base_criti.ksrc_para[3] * ksi1;
                    ksrc->pos[1] = base_criti.ksrc_para[1] + base_criti.ksrc_para[3] * ksi2;
                    ksrc->pos[2] = base_criti.ksrc_para[2] + base_criti.ksrc_para[3] * ksi3;
                }
            }
            break;
        }
        default:puts("unknown fission source type.");
            break;
    }

    base_RNG.position_pre = -1000;
    base_RNG.position = 0;

    for(i = 0; i < base_num_threads; i++) {
        ksrc_cnt = pth_args[i].src_cnt;
        for(j = 0; j < ksrc_cnt; j++) {
            get_rand_seed_host(&base_RNG);
            ksi1 = get_rand_host(&base_RNG);
            ksi2 = get_rand_host(&base_RNG);
            ksrc = &pth_args[i].src[j];
            ksrc->dir[0] = TWO * ksi2 - ONE;
            ksrc->dir[1] = sqrt(ONE - SQUARE(ksrc->dir[0])) * cos(TWO * PI * ksi1);
            ksrc->dir[2] = sqrt(ONE - SQUARE(ksrc->dir[0])) * sin(TWO * PI * ksi1);

            double T = 4.0 / 3.0;
            ksrc->erg = sample_maxwell_host(&base_RNG, T);
        }
    }

    base_start_wgt = ONE;

    base_RNG.position_pre = -1000;
    base_RNG.position = 0;

    puts("Finished.");
}