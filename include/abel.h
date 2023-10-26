#ifndef ABEL_H
#define ABEL_H

/* STL includes */
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* SDL includes */
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>

/* broadly used types */
typedef struct _tAbelT_task tAbelT_task;
typedef struct _tAbelG_game tAbelG_game;
typedef struct _tAbelC_chunk tAbelC_chunk;
typedef struct _tAbelR_texture tAbelR_texture;
typedef struct _tAbelS_sprite tAbelS_sprite;
typedef struct _tAbelE_entity tAbelE_entity;
typedef struct _tAbelM_cell tAbelM_cell;

typedef uint16_t ASSET_ID;
typedef uint8_t LAYER_ID;
typedef uint16_t TILE_ID;
typedef uint32_t SPRITE_ID;
typedef uint32_t ENTITY_ID;

#endif