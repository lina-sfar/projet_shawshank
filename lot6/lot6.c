#include "puzzle.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/* ============================================================
   HELPER FUNCTIONS
   ============================================================ */

/* crop one piece out of the full image */
static SDL_Texture *crop_piece(SDL_Renderer *r, SDL_Surface *full, int idx)
{
    int fw = full->w / 3;
    int fh = full->h / 3;
    int col = idx % 3;
    int row = idx / 3;

    /* SDL_PIXELFORMAT_RGBA32 is readable and avoids raw hex masks */
    SDL_Surface *part = SDL_CreateRGBSurfaceWithFormat(0, fw, fh, 32, SDL_PIXELFORMAT_RGBA32);
    SDL_Rect src = { col * fw, row * fh, fw, fh };
    SDL_BlitSurface(full, &src, part, NULL);

    SDL_Texture *t = SDL_CreateTextureFromSurface(r, part);
    SDL_FreeSurface(part);
    return t;
}

/* draw a filled rectangle with rounded corners */
static void draw_rounded_rect(SDL_Renderer *r, SDL_Rect rect, int radius,
    Uint8 rv, Uint8 gv, Uint8 bv, Uint8 av)
{
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, rv, gv, bv, av);

    SDL_Rect inner;
    inner.x = rect.x + radius;
    inner.y = rect.y;
    inner.w = rect.w - 2 * radius;
    inner.h = rect.h;
    SDL_RenderFillRect(r, &inner);

    inner.x = rect.x;
    inner.y = rect.y + radius;
    inner.w = rect.w;
    inner.h = rect.h - 2 * radius;
    SDL_RenderFillRect(r, &inner);

    int x, y;
    for (y = 0; y < radius; y++) {
        for (x = 0; x < radius; x++) {
            int dx = radius - x - 1;
            int dy = radius - y - 1;
            if (dx * dx + dy * dy <= radius * radius) {
                SDL_RenderDrawPoint(r, rect.x + x,           rect.y + y);
                SDL_RenderDrawPoint(r, rect.x + rect.w-1-x,  rect.y + y);
                SDL_RenderDrawPoint(r, rect.x + x,           rect.y + rect.h-1-y);
                SDL_RenderDrawPoint(r, rect.x + rect.w-1-x,  rect.y + rect.h-1-y);
            }
        }
    }
}

/* draw text centered at (cx, cy) */
static void draw_text_centered(SDL_Renderer *r, TTF_Font *font,
    const char *text, SDL_Color col, int cx, int cy)
{
    if (!font) return;
    SDL_Surface *s = TTF_RenderUTF8_Blended(font, text, col);
    if (!s) return;
    SDL_Texture *t = SDL_CreateTextureFromSurface(r, s);
    SDL_Rect dst = { cx - s->w / 2, cy - s->h / 2, s->w, s->h };
    SDL_FreeSurface(s);
    SDL_RenderCopy(r, t, NULL, &dst);
    SDL_DestroyTexture(t);
}

/* draw a filled triangle using scanline fill */
static void fill_triangle(SDL_Renderer *r,
    int x0, int y0, int x1, int y1, int x2, int y2)
{
    /* sort vertices by y (bubble sort, 3 points) */
    int tx, ty;
    if (y0 > y1) { tx=x0; ty=y0; x0=x1; y0=y1; x1=tx; y1=ty; }
    if (y1 > y2) { tx=x1; ty=y1; x1=x2; y1=y2; x2=tx; y2=ty; }
    if (y0 > y1) { tx=x0; ty=y0; x0=x1; y0=y1; x1=tx; y1=ty; }

    int y;
    for (y = y0; y <= y2; y++) {
        /* compute left and right x for this scanline */
        float t1, t2, lx, rx;
        if (y <= y1) {
            if (y1 == y0) {
                lx = (float)(x0 < x1 ? x0 : x1);
                rx = (float)(x0 > x1 ? x0 : x1);
            } else {
                t1 = (float)(y - y0) / (y1 - y0);
                t2 = (float)(y - y0) / (y2 - y0);
                lx = x0 + t1 * (x1 - x0);
                rx = x0 + t2 * (x2 - x0);
            }
        } else {
            t1 = (float)(y - y1) / (y2 - y1);
            t2 = (float)(y - y0) / (y2 - y0);
            lx = x1 + t1 * (x2 - x1);
            rx = x0 + t2 * (x2 - x0);
        }
        if (lx > rx) { float tmp = lx; lx = rx; rx = tmp; }
        SDL_RenderDrawLine(r, (int)lx, y, (int)rx, y);
    }
}

/* ============================================================
   HOURGLASS DRAWING
   Drawn in the header area, top-right corner.
   cx, cy = center of the hourglass
   progress = 0.0 (full) to 1.0 (empty) — sand has fallen
   ============================================================ */
static void draw_hourglass(SDL_Renderer *r, int cx, int cy, float progress,
    int time_up, int remaining)
{
    int hw = 22;   /* half-width of hourglass at widest point */
    int hh = 24;   /* half-height of hourglass */
    int neck = 3;  /* half-width of the neck */

    /* ---------- OUTER FRAME (dark background) ---------- */
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, 0, 0, 0, 80);
    SDL_Rect frame = { cx - hw - 6, cy - hh - 6, (hw + 6) * 2, (hh + 6) * 2 };
    SDL_RenderFillRect(r, &frame);

    /* ---------- BORDER OUTLINE ---------- */
    SDL_SetRenderDrawColor(r, 255, 255, 255, 40);
    SDL_RenderDrawRect(r, &frame);

    /* ---------- GLASS OUTLINE (hourglass shape) ---------- */
    /* We draw the outline as 4 lines forming the X shape */
    Uint8 outline_r = time_up ? 255 : 200;
    Uint8 outline_g = time_up ?  80 : 200;
    Uint8 outline_b = time_up ?  80 : 200;
    SDL_SetRenderDrawColor(r, outline_r, outline_g, outline_b, 180);

    /* top triangle outline */
    SDL_RenderDrawLine(r, cx - hw, cy - hh,  cx - neck, cy);   /* left side */
    SDL_RenderDrawLine(r, cx + hw, cy - hh,  cx + neck, cy);   /* right side */
    SDL_RenderDrawLine(r, cx - hw, cy - hh,  cx + hw,   cy - hh); /* top edge */
    /* bottom triangle outline */
    SDL_RenderDrawLine(r, cx - neck, cy,     cx - hw,   cy + hh); /* left side */
    SDL_RenderDrawLine(r, cx + neck, cy,     cx + hw,   cy + hh); /* right side */
    SDL_RenderDrawLine(r, cx - hw,   cy + hh, cx + hw,  cy + hh); /* bottom edge */

    /* ---------- SAND IN UPPER CHAMBER (remaining sand) ---------- */
    /* sand fills from the bottom of the upper chamber upward */
    float sand_top = 1.0f - progress;  /* how full is the top (1=full, 0=empty) */

    if (sand_top > 0.01f) {
        /* the top chamber is a triangle: apex at (cx,cy), base at top (cy-hh) */
        /* at a given height fraction, the width scales linearly */
        /* sand sits at the bottom of the top triangle */
        int sand_y_top = cy - (int)(hh * sand_top);
        int sand_w_top = (int)(hw * sand_top) - 1;
        if (sand_w_top < 1) sand_w_top = 1;

        SDL_SetRenderDrawColor(r, 210, 180, 100, 200);
        fill_triangle(r,
            cx - sand_w_top, sand_y_top,
            cx + sand_w_top, sand_y_top,
            cx, cy);              /* apex of top triangle at neck */
    }

    /* ---------- SAND IN LOWER CHAMBER (fallen sand) ---------- */
    if (progress > 0.01f) {
        /* the bottom chamber is a triangle: apex at (cx,cy), base at bottom */
        /* sand fills from the base upward based on progress */
        int sand_h = (int)(hh * progress);
        int sand_base_y = cy + hh;
        int sand_top_y  = sand_base_y - sand_h;
        /* at sand_top_y the width of the triangle = hw * (sand_h / hh) */
        int sand_w_bot  = (int)(hw * progress) - 1;
        if (sand_w_bot < 1) sand_w_bot = 1;

        SDL_SetRenderDrawColor(r, 210, 180, 100, 200);
        fill_triangle(r,
            cx - sand_w_bot, sand_top_y,
            cx + sand_w_bot, sand_top_y,
            cx - hw, cy + hh);   /* doesn't matter, scanline fill corrects it */

        /* re-draw as a proper trapezoid: flat top, triangular base */
        /* easier: fill from sand_top_y to cy+hh using the triangle apex at cx,cy */
        /* override with correct fill */
        SDL_SetRenderDrawColor(r, 210, 180, 100, 200);
        int y;
        for (y = sand_top_y; y <= cy + hh; y++) {
            float frac = (float)(y - cy) / (float)hh;  /* 0 at neck, 1 at bottom */
            if (frac < 0.0f) frac = 0.0f;
            int half_w = (int)(hw * frac);
            SDL_RenderDrawLine(r, cx - half_w, y, cx + half_w, y);
        }
    }

    /* ---------- FALLING SAND GRAIN (animated dot at neck) ---------- */
    if (!time_up && progress > 0.01f && progress < 0.99f) {
        Uint32 ticks = SDL_GetTicks();
        int grain_y = cy + (int)((ticks / 80) % 12);  /* falls 12px then resets */
        SDL_SetRenderDrawColor(r, 255, 220, 120, 220);
        SDL_RenderDrawPoint(r, cx,     grain_y);
        SDL_RenderDrawPoint(r, cx + 1, grain_y);
        SDL_RenderDrawPoint(r, cx,     grain_y + 1);
    }

    /* ---------- NUMBER COUNTDOWN BELOW HOURGLASS ---------- */
    /* We draw a small number under the frame */
    (void)remaining; /* drawn via text outside this function */

    /* ---------- PULSING RED FRAME when time is low ---------- */
    if (!time_up && remaining <= 3) {
        Uint32 t = SDL_GetTicks();
        float pulse = (float)(t % 600) / 600.0f;
        Uint8 alpha = (Uint8)(80 + 120 * pulse);
        SDL_SetRenderDrawColor(r, 255, 80, 80, alpha);
        SDL_RenderDrawRect(r, &frame);
        SDL_Rect frame2 = { frame.x - 1, frame.y - 1, frame.w + 2, frame.h + 2 };
        SDL_RenderDrawRect(r, &frame2);
    }
}

/* ============================================================
   GAME LOGIC
   ============================================================ */

static SDL_Rect hole_rect(Game *g)
{
    SDL_Rect h;
    h.x = GRID_X + (g->missing % 3) * PIECE_W;
    h.y = GRID_Y + 10 + (g->missing / 3) * PIECE_H;
    h.w = PIECE_W;
    h.h = PIECE_H;
    return h;
}

void free_game(Game *g)
{
    int i;
    if (g->full) { SDL_DestroyTexture(g->full); g->full = NULL; }
    for (i = 0; i < 9; i++) {
        if (g->pieces[i]) { SDL_DestroyTexture(g->pieces[i]); g->pieces[i] = NULL; }
    }
    memset(g->choices, 0, sizeof(g->choices));
}

int init_game(SDL_Renderer *r, Game *g)
{
    memset(g, 0, sizeof(Game));
    srand((unsigned)time(NULL));
    new_round(r, g);
    return 1;
}

void new_round(SDL_Renderer *r, Game *g)
{
    int i;
    free_game(g);

    g->img_index = rand() % 5;
    char path[32];
    sprintf(path, "img%d.png", g->img_index);

    SDL_Surface *full = IMG_Load(path);
    if (!full) { printf("IMG_Load error: %s\n", IMG_GetError()); return; }

    g->full = SDL_CreateTextureFromSurface(r, full);
    for (i = 0; i < 9; i++) g->pieces[i] = crop_piece(r, full, i);
    SDL_FreeSurface(full);

    g->missing  = rand() % 9;
    g->solved   = 0;
    g->dragging = -1;
    g->time_up  = 0;
    g->start_ticks = SDL_GetTicks();

    int wrong1, wrong2;
    do { wrong1 = rand() % 9; } while (wrong1 == g->missing);
    do { wrong2 = rand() % 9; } while (wrong2 == g->missing || wrong2 == wrong1);

    int pool[3] = { g->missing, wrong1, wrong2 };
    for (i = 2; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = pool[i]; pool[i] = pool[j]; pool[j] = tmp;
    }

    int choice_h = PIECE_H;
    int gap = 18;
    int total = 3 * choice_h + 2 * gap;
    int start_y = (WINDOW_H - total) / 2;

    for (i = 0; i < 3; i++) {
        g->choices[i].piece_index = pool[i];
        g->choices[i].is_correct  = (pool[i] == g->missing);
        g->choices[i].wrong_flash = 0;
        g->choices[i].visible     = 1;
        g->choices[i].texture     = g->pieces[pool[i]];

        SDL_Rect rr;
        rr.x = CHOICE_X;
        rr.y = start_y + i * (choice_h + gap);
        rr.w = PIECE_W;
        rr.h = PIECE_H;
        g->choices[i].rect = rr;
        g->choices[i].home = rr;
    }
}

/* ============================================================
   INPUT
   ============================================================ */

void handle_mousedown(Game *g, int mx, int my)
{
    if (g->solved || g->time_up || g->dragging != -1) return;
    int i;
    for (i = 0; i < 3; i++) {
        if (!g->choices[i].visible) continue;
        SDL_Point p = { mx, my };
        if (SDL_PointInRect(&p, &g->choices[i].rect)) {
            g->dragging = i;
            g->drag_ox  = mx - g->choices[i].rect.x;
            g->drag_oy  = my - g->choices[i].rect.y;
            g->choices[i].visible = 0;
            break;
        }
    }
}

void handle_mousemove(Game *g, int mx, int my)
{
    if (g->dragging == -1) return;
    g->choices[g->dragging].rect.x = mx - g->drag_ox;
    g->choices[g->dragging].rect.y = my - g->drag_oy;
}

void handle_mouseup(Game *g, int mx, int my)
{
    if (g->dragging == -1) return;
    int idx = g->dragging;
    g->dragging = -1;

    SDL_Rect h = hole_rect(g);
    SDL_Point p = { mx, my };

    if (SDL_PointInRect(&p, &h)) {
        if (g->choices[idx].is_correct) {
            g->choices[idx].rect = h;
            g->choices[idx].visible = 1;
            g->solved = 1;
        } else {
            g->choices[idx].rect      = g->choices[idx].home;
            g->choices[idx].visible   = 1;
            g->choices[idx].wrong_flash = SDL_GetTicks();
        }
    } else {
        g->choices[idx].rect    = g->choices[idx].home;
        g->choices[idx].visible = 1;
    }
}

/* ============================================================
   DRAWING
   ============================================================ */

void draw_game(SDL_Renderer *r, TTF_Font *font, TTF_Font *small, Game *g, int mx, int my)
{
    int i;

    /* background gradient */
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
    for (i = 0; i < WINDOW_H; i++) {
        int v = 18 + (i * 12 / WINDOW_H);
        SDL_SetRenderDrawColor(r, v, v + 2, v + 8, 255);
        SDL_RenderDrawLine(r, 0, i, WINDOW_W, i);
    }

    /* header bar */
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, 255, 255, 255, 14);
    SDL_Rect header = { 0, 0, WINDOW_W, 62 };
    SDL_RenderFillRect(r, &header);


    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color dimw  = { 180, 190, 210, 255 };
    draw_text_centered(r, font, "Drag the missing piece!", white, 265, 31);

    SDL_SetRenderDrawColor(r, 255, 255, 255, 25);
    SDL_RenderDrawLine(r, 0, 63, WINDOW_W, 63);

    /* --- HOURGLASS TIMER --- */
    Uint32 now = SDL_GetTicks();
    int elapsed   = (int)((now - g->start_ticks) / 1000);
    int remaining = TIMER_SECONDS - elapsed;
    if (remaining < 0) remaining = 0;

    /* update time_up flag */
    if (!g->solved && !g->time_up && remaining == 0)
        g->time_up = 1;

    /* progress = how much sand has FALLEN (0=none fallen, 1=all fallen) */
    float progress = (float)(TIMER_SECONDS - remaining) / (float)TIMER_SECONDS;
    if (progress > 1.0f) progress = 1.0f;

    /* hourglass in top-right corner of the window (x=730, header height=62) */
    int hg_cx = 730;
    int hg_cy = 31;

    if (!g->solved && !g->time_up) {
        draw_hourglass(r, hg_cx, hg_cy, progress, 0, remaining);

        /* number to the right of the hourglass */
        char num[8];
        sprintf(num, "%d", remaining);
        SDL_Color num_color = remaining <= 3
            ? (SDL_Color){ 255, 80, 80, 255 }
            : (SDL_Color){ 255, 220, 100, 255 };
        draw_text_centered(r, small, num, num_color, hg_cx, hg_cy);
    } else if (g->time_up) {
        /* show empty red hourglass */
        draw_hourglass(r, hg_cx, hg_cy, 1.0f, 1, 0);
    }

    /* --- PUZZLE GRID --- */
    int gx = GRID_X;
    int gy = GRID_Y + 10;

    for (i = 0; i < 9; i++) {
        int col = i % 3, row = i / 3;
        SDL_Rect dest;
        dest.x = gx + col * PIECE_W;
        dest.y = gy + row * PIECE_H;
        dest.w = PIECE_W;
        dest.h = PIECE_H;

        if (i == g->missing && !g->solved) {
            /* draw empty hole */
            SDL_SetRenderDrawColor(r, 8, 10, 18, 255);
            SDL_RenderFillRect(r, &dest);

            /* pulsing dashed border */
            Uint32 t = SDL_GetTicks();
            int dash_off = (t / 120) % 10;
            SDL_SetRenderDrawColor(r, 255, 200, 60, 180);
            int d;
            for (d = -dash_off; d < dest.w; d += 10) {
                int x1 = dest.x + d, x2 = dest.x + d + 5;
                if (x1 < dest.x) x1 = dest.x;
                if (x2 > dest.x + dest.w) x2 = dest.x + dest.w;
                if (x2 > x1) {
                    SDL_RenderDrawLine(r, x1, dest.y, x2, dest.y);
                    SDL_RenderDrawLine(r, x1, dest.y + dest.h - 1, x2, dest.y + dest.h - 1);
                }
            }
            for (d = -dash_off; d < dest.h; d += 10) {
                int y1 = dest.y + d, y2 = dest.y + d + 5;
                if (y1 < dest.y) y1 = dest.y;
                if (y2 > dest.y + dest.h) y2 = dest.y + dest.h;
                if (y2 > y1) {
                    SDL_RenderDrawLine(r, dest.x, y1, dest.x, y2);
                    SDL_RenderDrawLine(r, dest.x + dest.w - 1, y1, dest.x + dest.w - 1, y2);
                }
            }

            draw_text_centered(r, font, "?",
                (SDL_Color){ 255, 200, 60, 200 },
                dest.x + dest.w / 2, dest.y + dest.h / 2);

        } else {
            SDL_SetRenderDrawColor(r, 255, 255, 255, 18);
            SDL_Rect br = { dest.x - 1, dest.y - 1, dest.w + 2, dest.h + 2 };
            SDL_RenderFillRect(r, &br);
            SDL_RenderCopy(r, g->pieces[i], NULL, &dest);
        }
    }

    /* green glow when solved */
    if (g->solved) {
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(r, 80, 220, 120, 160);
        SDL_Rect outline = { gx - 3, gy - 3, PIECE_W * 3 + 6, PIECE_H * 3 + 6 };
        SDL_RenderDrawRect(r, &outline);
        SDL_RenderDrawRect(r, &(SDL_Rect){ gx - 4, gy - 4, PIECE_W * 3 + 8, PIECE_H * 3 + 8 });
    }

    /* divider and label */
    SDL_SetRenderDrawColor(r, 255, 255, 255, 30);
    SDL_RenderDrawLine(r, CHOICE_X - 20, 75, CHOICE_X - 20, WINDOW_H - 10);
    draw_text_centered(r, small, "Drag one here:", dimw, CHOICE_X + PIECE_W / 2, 72);

    /* --- CHOICE PIECES --- */
    for (i = 0; i < 3; i++) {
        Choice *c = &g->choices[i];
        if (!c->visible) continue;

        Uint8 br2 = 255, bg2 = 255, bb2 = 255, ba2 = 30;

        if (c->wrong_flash != 0 && now - c->wrong_flash < 600) {
            float t2 = 1.0f - (float)(now - c->wrong_flash) / 600.0f;
            ba2 = (Uint8)(200 * t2);
            br2 = 220; bg2 = 60; bb2 = 60;
        } else {
            SDL_Point mp = { mx, my };
            if (SDL_PointInRect(&mp, &c->rect) && g->dragging == -1) {
                br2 = 255; bg2 = 200; bb2 = 60; ba2 = 200;
            }
        }

        SDL_Rect card   = { c->rect.x - 4, c->rect.y - 4, c->rect.w + 8, c->rect.h + 8 };
        SDL_Rect shadow = { card.x + 4, card.y + 4, card.w, card.h };
        draw_rounded_rect(r, shadow, 8, 0, 0, 0, 70);
        draw_rounded_rect(r, card, 8, br2, bg2, bb2, ba2);

        SDL_RenderCopy(r, c->texture, NULL, &c->rect);

        /* letter badge A/B/C */
        char letter[3] = { 'A' + i, '\0' };
        SDL_Rect badge = { c->rect.x, c->rect.y, 22, 22 };
        SDL_SetRenderDrawColor(r, 20, 20, 35, 210);
        SDL_RenderFillRect(r, &badge);
        draw_text_centered(r, small, letter, (SDL_Color){ 255, 200, 60, 255 },
            badge.x + 11, badge.y + 11);

        if (c->wrong_flash != 0 && now - c->wrong_flash < 600) {
            float t2 = 1.0f - (float)(now - c->wrong_flash) / 600.0f;
            Uint8 alpha = (Uint8)(255 * t2);
            draw_text_centered(r, small, "Wrong!",
                (SDL_Color){ 255, 80, 80, alpha },
                c->rect.x + c->rect.w / 2,
                c->rect.y + c->rect.h + 12);
        }
    }

    /* --- DRAGGED PIECE (on top) --- */
    if (g->dragging != -1) {
        Choice *c = &g->choices[g->dragging];
        SDL_Rect dshadow = { c->rect.x + 6, c->rect.y + 6, c->rect.w, c->rect.h };
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(r, 0, 0, 0, 100);
        SDL_RenderFillRect(r, &dshadow);

        SDL_Rect glow = { c->rect.x - 4, c->rect.y - 4, c->rect.w + 8, c->rect.h + 8 };
        draw_rounded_rect(r, glow, 8, 255, 200, 60, 200);
        SDL_RenderCopy(r, c->texture, NULL, &c->rect);
    }

    /* --- TIME IS UP OVERLAY --- */
    if (g->time_up) {
        int cx2 = gx + PIECE_W * 3 / 2;
        int cy2 = gy + PIECE_H * 3 / 2;

        SDL_Rect bg2 = { cx2 - 160, cy2 - 50, 320, 100 };
        draw_rounded_rect(r, bg2, 14, 10, 12, 22, 230);

        draw_text_centered(r, font, "Time is over!",
            (SDL_Color){ 255, 80, 80, 255 }, cx2, cy2 - 14);
        draw_text_centered(r, small, "Press T to try again",
            (SDL_Color){ 180, 190, 210, 255 }, cx2, cy2 + 26);
    }

    /* --- SOLVED OVERLAY --- */
    if (g->solved) {
        int cx2 = gx + PIECE_W * 3 / 2;
        int cy2 = gy + PIECE_H * 3 / 2;

        SDL_Rect bg2 = { cx2 - 140, cy2 - 38, 280, 76 };
        draw_rounded_rect(r, bg2, 14, 10, 12, 22, 230);

        draw_text_centered(r, font, "Correct!",
            (SDL_Color){ 80, 220, 120, 255 }, cx2, cy2 - 12);
        draw_text_centered(r, small, "Press X to close",
            (SDL_Color){ 180, 190, 210, 255 }, cx2, cy2 + 24);
    }
}
