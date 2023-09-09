#include <stdio.h>
#include <asigraph.h>

int main(void) {

    aginit((aginit_arg){
        .constructor = AG_DEFAULT_CONSTRUCTOR,
        .destructor = {
            .abnormal = AG_DEFAULT_ABNORMAL_DESTRUCTOR,
            .normal = AG_DEFAULT_NORMAL_DESTRUCTOR
        }
    });

    // W some bs

    aginit((aginit_arg){
        .constructor = ^(void){
            initscr();
            cbreak();
            noecho();
        }
        .destructor = {
            .abnormal = ^(int sig){
                agexhndlsig = sig;
                exit(EXIT_FAILURE);
            },
            .normal = ^(void) {
                endwin();
                if(agexhndlsig != 0) {
                    while(viwerr(VIWERR_OCCURED, NULL)){
                        viwerr(VIWERR_PRINT, NULL);
                    }
                }
                return;
            }
        }
    });

    // OR

    aginit(AGINIT_DEFAULT);

    // ORR

    aginit((aginit_arg){0});

    agcont_t = agcontinit((agcont_t){
        .left = {
            .position = ^(int  right, int xmax) { ... },
            .padding = 0,
        },
        .right = {
            .position = ^(int   left, int xmax) { ... },
            .padding = 0
        },
        .top = {
            .position = ^(int bottom, int ymax) { ... },
            .padding = 0
        },
        .bottom = {
            .position = ^(int top, int ymax) { ... },
            .padding = 0
        },
        .border = {
            .draw = true,
            .icons = {
                .top    = '-',
                .bottom = '-',
                .left   = '|',
                .right  = '|',
                .tr     = '+', 
                .tl     = '+', 
                .br     = '+', 
                .bl     = '+' 
            },
            // .collision = {
            //     .merge = true,
            //     .icons = {
            //         ...
            //     }
            // }
        },
        .flag = {
            .render = true,
        }
        // .attributes = {
        //     .menu = agmenuinit(4)
        // }
        // .widgets = agobjlist(1,
        //     (agobj_t*)agmenuinit((agmenu_t){
        //         // .entries    = {"File", "Draw", "Quit"},
        //         .entry = agmenuentlist(
        //             (agmenuent_t*)agmenuentinit("File",)
        //         ),
        //         .position   = AGMENU_POSITION_LEFT_BOTTOM,
        //         .scrollable = true,
        //         .spacing    = 1,
        //         .length     = 12,
        //     }),
        // );

    });

    double fr, dt;
    while(agtimer(0.00, &fr, &dt)) {



    }
}