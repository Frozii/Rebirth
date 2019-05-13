#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define internal static
#define global static

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float r32;
typedef double r64;

typedef u32 b32;

#define ROOM_WIDTH 24
#define ROOM_HEIGHT 10

#define MAX_LENGTH 256
#define GENERAL_LENGHT 32

#define ITEM_COUNT 20
#define SEARCHABLE_COUNT 11
#define LOOT_COUNT 3

#define ASCII_LOWERCASE_START 96

enum
{
  key_enter = 10,
  key_up_arrow = KEY_UP,
  key_down_arrow = KEY_DOWN,
} key_e;

enum
{
  black_pair,
  red_pair,
  green_pair,
  yellow_pair,
  blue_pair,
  magenta_pair,
  cyan_pair,
  white_pair,

  stone_pair,
  wood_pair,
  metal_pair,
  light_pair,
  grey_pair,
  white_on_grey_pair, // NOTE(Rami): Do we need this pair?

  background_grey_pair,

  // NOTE(Rami): Remove later!
  pair_count
} color_pair_e;

enum
{
  color_stone = 8,
  color_wood,
  color_metal,
  color_light,
  color_grey,
} color_e;

enum
{
  glyph_blank = ' ',
  glyph_stone = '#',
  glyph_floor = '.',
  glyph_bookshelf = 'B',
  glyph_crate = 'X',
  glyph_small_crate = 'x',
  glyph_stone_door = '|',
  glyph_stone_door_open = '_',
  glyph_wooden_door = '+',
  glyph_wooden_door_open = '/',
  glyph_open_chest = 'C',
  glyph_table = 'T',
  glyph_chair = 'L',
  glyph_torch = 'i',
  glyph_vial = '!',
  glyph_tin = '}',
  glyph_magnet = ']',
  glyph_chain = '~',
  glyph_knife = 'I',
  glyph_metal_spade = 'S',
  glyph_metal_spade_no_handle = 's',
  glyph_bunsen_burner = '^',
  glyph_bronze_key = '='
} glyph_e;

typedef enum
{
  item_none,
  item_metal_spade,
  item_metal_spade_no_handle,
  item_knife,
  item_empty_vial,
  item_dihydrogen_monoxide,
  item_cupric_ore_powder,
  item_tin_ore_powder,
  item_tin,
  item_sodium_chloride,
  item_gypsum,
  item_cupric_sulfate,
  item_acetic_acid,
  item_magnet,
  item_bunsen_burner,
  item_bronze_key,
  item_count
} item_e;

typedef enum
{
  state_main_menu,
  state_play,
  state_controls,
  state_quit
} game_state_e;

typedef struct
{
  game_state_e state;

  i32 menu_option_selected;
  i32 menu_option_count;

  b32 first_door_open;
  b32 first_door_dihydrogen_monoxide_added;
  b32 first_door_cupric_sulfate_added;
  b32 first_door_spade_inserted;

  b32 second_door_open;
  b32 second_door_key_inserted;
  b32 second_door_key_pried;
  b32 second_door_key_complete;
  b32 second_door_tin_ore_powder_added;
  b32 second_door_cupric_ore_powder_added;
  b32 second_door_key_imprint_made;
  b32 second_door_gypsum_added;
  b32 second_door_dihydrogen_monoxide_added;
} game_t;

typedef struct
{
  b32 active;
  b32 in_inventory;
  item_e type;
  char name[GENERAL_LENGHT];
  i32 id;
  i32 x;
  i32 y;
  char glyph;
} item_t;

typedef struct
{
  char name[GENERAL_LENGHT];
  i32 hp;
  i32 x;
  i32 y;
  i32 turn;
  i32 input;

  b32 using_an_item;
  b32 interacting;
  b32 inspecting;
  b32 picking_up;

  item_t inventory[ITEM_COUNT];
  i32 inventory_first_combination_item_num;
  i32 inventory_second_combination_item_num;
  item_e inventory_first_combination_item;
  item_e inventory_second_combination_item;
  b32 inventory_enabled;
  i32 inventory_item_selected;
  i32 inventory_item_count;
} player_t;

typedef struct
{
  b32 searched;
  i32 x;
  i32 y;
  item_e loot[LOOT_COUNT];
} searchable_t;

global game_t game;
global player_t player;
global u8 room[ROOM_WIDTH][ROOM_HEIGHT];
global item_t items[ITEM_COUNT];
global searchable_t searchables[SEARCHABLE_COUNT];

internal void
move_menu_option_selected_up()
{
  if((game.menu_option_selected - 1) >= 1)
  {
    game.menu_option_selected--;
  }
}

internal void
move_menu_option_selected_down()
{
  if((game.menu_option_selected + 1) <= game.menu_option_count)
  {
    game.menu_option_selected++;
  }
}

internal void
update_main_menu()
{
  i32 input = getch();
  switch(input)
  {
    case key_enter:
    {
      if(game.menu_option_selected == 1)
      {
        game.state = state_play;
      }
      else if(game.menu_option_selected == 2)
      {
        // NOTE(Rami): Implement
        // game.state = state_controls;
        game.state = state_quit;
      }
      else
      {
        game.state = state_quit;
      }
    } break;

    case key_up_arrow: move_menu_option_selected_up(); break;
    case key_down_arrow: move_menu_option_selected_down(); break;
    default: break;
  }
}

internal void
render_main_menu()
{
  mvprintw(5, 10, " _____  _____  _____  _____  _____  _____  _   _ ");
  mvprintw(6, 10, "|  _  \\|  ___||  _  \\|_   _||  _  \\|_   _|| | | |");
  mvprintw(7, 10, "| |_| /| |__  | |_| /  | |  | |_| /  | |  | |_| |");
  mvprintw(8, 10, "| .  / |  __| |  _  \\  | |  | .  /   | |  |  _  |");
  mvprintw(9, 10, "| |\\ \\ | |___ | |_| / _| |_ | |\\ \\   | |  | | | |");
  mvprintw(10, 10, "\\_| \\_|\\____/ \\____/  \\___/ \\_| \\_|  \\_/  \\_| |_/");

  if(game.menu_option_selected == 1)
  {
    attron(COLOR_PAIR(cyan_pair));
    mvprintw(14, 10, "Play");
    attroff(COLOR_PAIR(cyan_pair));

    mvprintw(15, 10, "Controls");
    mvprintw(16, 10, "Exit");
  }
  else if(game.menu_option_selected == 2)
  {
    mvprintw(14, 10, "Play");

    attron(COLOR_PAIR(cyan_pair));
    mvprintw(15, 10, "Controls");
    attroff(COLOR_PAIR(cyan_pair));

    mvprintw(16, 10, "Exit");
  }
  else
  {
    mvprintw(14, 10, "Play");
    mvprintw(15, 10, "Controls");

    attron(COLOR_PAIR(cyan_pair));
    mvprintw(16, 10, "Exit");
    attroff(COLOR_PAIR(cyan_pair));
  }
}

internal i32
is_valid_input(i32 key)
{
  if(key == 'w' || key == 'W' ||
     key == 'a' || key == 'A' ||
     key == 's' || key == 'S' ||
     key == 'd' || key == 'D' ||
     key == 'b' || key == 'B' ||
     key == 'c' || key == 'C' ||
     key == 'p' || key == 'P' ||
     key == 'o' || key == 'O' ||
     key == 'i' || key == 'I' ||
     key == 'u' || key == 'U' ||
     key == 'y' || key == 'Y' ||
     key == 'q' || key == 'Q')
  {
    return 1;
  }

  return 0;
}

internal i32
get_item_type_for_inventory_position(i32 i)
{
  return player.inventory[i - 1].type;
}

// NOTE(Rami): Do we need this?
// internal i32 item_in_inventory(item_e type)
// {
//   for(i32 i = 0; i < ITEM_COUNT; i++)
//   {
//     if(player.inventory[i].in_inventory &&
//        player.inventory[i].type == type)
//     {
//       return 1;
//     }
//   }

//   return 0;
// }

// NOTE(Rami): Do we need this?
// internal i32 get_inventory_position_for_item_type(item_e type)
// {
//   for(i32 i = 0; i < ITEM_COUNT; i++)
//   {
//     if(player.inventory[i].type == type)
//     {
//       return i + 1;
//     }
//   }

//   return -1;
// }

internal i32
is_traversable(i32 x, i32 y)
{
  // NOTE(Rami): FOR DEBUGGING
  return 1;

  if(room[x][y] == glyph_floor ||
     room[x][y] == glyph_stone_door_open ||
     room[x][y] == glyph_wooden_door_open)
  {
    return 1;
  }

  return 0;
}

internal inline i32
equal_pos(i32 ax, i32 ay, i32 bx, i32 by)
{
  if(ax == bx && ay == by)
  {
    return 1;
  }

  return 0;
}

internal item_e
get_item_type_for_pos(i32 x, i32 y)
{
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    if(equal_pos(x, y, items[i].x, items[i].y))
    {
      return items[i].type;
    }
  }

  return item_none;
}

internal i32
get_item_pos_for_id(i32 id)
{
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    if(items[i].id == id)
    {
      return i;
    }
  }

  return -1;
}

internal void
get_item_name_for_item_type(char *storage, i32 type)
{
  switch(type)
  {
    case item_metal_spade: strcpy(storage, "Metal Spade"); break;
    case item_metal_spade_no_handle: strcpy(storage, "Metal Spade (No Handle)"); break;
    case item_knife: strcpy(storage, "Knife"); break;
    case item_empty_vial: strcpy(storage, "Empty Vial"); break;
    case item_dihydrogen_monoxide: strcpy(storage, "Dihydrogen Monoxide"); break;
    case item_cupric_ore_powder: strcpy(storage, "Cupric Ore Powder"); break;
    case item_tin_ore_powder: strcpy(storage, "Tin Ore Powder"); break;
    case item_tin: strcpy(storage, "Tin"); break;
    case item_sodium_chloride: strcpy(storage, "Sodium Chloride"); break;
    case item_gypsum: strcpy(storage, "Gypsum"); break;
    case item_cupric_sulfate: strcpy(storage, "Cupric Sulfate"); break;
    case item_acetic_acid: strcpy(storage, "Acetic Acid"); break;
    case item_magnet: strcpy(storage, "Magnet"); break;
    case item_bunsen_burner: strcpy(storage, "Bunsen Burner"); break;
    case item_bronze_key: strcpy(storage, "Bronze Key"); break;
  }
}

internal char
get_item_glyph_for_item_type(i32 type)
{
  char val = 0;

  switch(type)
  {
    case item_metal_spade: return glyph_metal_spade; break;
    case item_metal_spade_no_handle: return glyph_metal_spade_no_handle; break;
    case item_knife: return glyph_knife; break;
    case item_empty_vial: return glyph_vial; break;
    case item_dihydrogen_monoxide: return glyph_vial; break;
    case item_cupric_ore_powder: return glyph_vial; break;
    case item_tin_ore_powder: return glyph_vial; break;
    case item_tin: return glyph_tin; break;
    case item_sodium_chloride: return glyph_vial; break;
    case item_gypsum: return glyph_vial; break;
    case item_cupric_sulfate: return glyph_vial; break;
    case item_acetic_acid: return glyph_vial; break;
    case item_magnet: return glyph_magnet; break;
    case item_bunsen_burner: return glyph_bunsen_burner; break;
    case item_bronze_key: return glyph_bronze_key; break;
  }

  return val;
}

internal void
render_items()
{
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    if(items[i].active)
    {
      char c[2] = {0};
      c[0] = items[i].glyph;
      mvprintw(items[i].y, items[i].x, c);
    }
  }
}

internal void
clear_message()
{
  char clear[MAX_LENGTH] = {0};
  memset(clear, ' ', sizeof(clear) - 1);

  for(i32 i = 0; i < 6; i++)
  {
    mvprintw(15 + i, 0, clear);
  }
}

internal void
render_message(char *msg, ...)
{
  char formatted_message[MAX_LENGTH];

  va_list arg_list;
  va_start(arg_list, msg);
  vsnprintf(formatted_message, sizeof(formatted_message), msg, arg_list);
  va_end(arg_list);

  mvprintw(15, 0, "> %s", formatted_message);
}

internal void
remove_inventory_item(i32 selected)
{
  i32 id_to_remove = player.inventory[selected - 1].id;

  // Remove item from game
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    if(items[i].id == id_to_remove)
    {
      items[i].active = false;
      items[i].in_inventory = false;
      items[i].type = item_none;
      memset(&items[i].name, 0, GENERAL_LENGHT - 1);
      items[i].id = 0;
      items[i].x = 0;
      items[i].y = 0;
      items[i].glyph = glyph_blank;
      break;
    }
  }

  // Reorder data
  for(i32 i = 1; i < ITEM_COUNT; i++)
  {
    if(items[i].active || items[i].in_inventory)
    {
      if(!items[i - 1].active && !items[i - 1].in_inventory)
      {
        items[i - 1] = items[i];

        items[i].active = false;
        items[i].in_inventory = false;
        items[i].type = item_none;
        memset(&items[i].name, 0, GENERAL_LENGHT - 1);
        items[i].id = 0;
        items[i].x = 0;
        items[i].y = 0;
        items[i].glyph = glyph_blank;
      }
    }
  }

  // Remove item from inventory
  player.inventory[selected - 1].active = false;
  player.inventory[selected - 1].in_inventory = false;
  player.inventory[selected - 1].type = item_none;
  memset(&player.inventory[selected - 1], 0, GENERAL_LENGHT - 1);
  player.inventory[selected - 1].id = 0;
  player.inventory[selected - 1].x = 0;
  player.inventory[selected - 1].y = 0;
  player.inventory[selected - 1].glyph = glyph_blank;

  // Reorder data
  for(i32 i = 1; i < ITEM_COUNT; i++)
  {
    if(player.inventory[i].in_inventory)
    {
      if(!player.inventory[i - 1].in_inventory)
      {
        player.inventory[i - 1] = player.inventory[i];

        player.inventory[i].active = false;
        player.inventory[i].in_inventory = false;
        player.inventory[i].type = item_none;
        memset(&player.inventory[i].name, 0, GENERAL_LENGHT - 1);
        player.inventory[i].id = 0;
        player.inventory[i].x = 0;
        player.inventory[i].y = 0;
        player.inventory[i].glyph = glyph_blank;
      }
    }
  }

  // Adjust highlighter
  if((player.inventory_item_selected - 1) >= 1)
  {
    player.inventory_item_selected--;
  }
}

internal void
drop_inventory_item(i32 x, i32 y, i32 selected)
{
  i32 id_to_enable = player.inventory[selected - 1].id;

  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    if(items[i].id == id_to_enable)
    {
      items[i].active = true;
      items[i].in_inventory = false;
      items[i].x = x;
      items[i].y = y;
    }
  }

  player.inventory[selected - 1].active = false;
  player.inventory[selected - 1].in_inventory = false;
  player.inventory[selected - 1].type = item_none;
  memset(&player.inventory[selected - 1].name, 0, GENERAL_LENGHT - 1);
  player.inventory[selected - 1].id = 0;
  player.inventory[selected - 1].x = 0;
  player.inventory[selected - 1].y = 0;
  player.inventory[selected - 1].glyph = glyph_blank;

  for(i32 i = 1; i < ITEM_COUNT; i++)
  {
    if(player.inventory[i].in_inventory)
    {
      if(!player.inventory[i - 1].in_inventory)
      {
        player.inventory[i - 1] = player.inventory[i];

        player.inventory[i].active = false;
        player.inventory[i].in_inventory = false;
        player.inventory[i].type = item_none;
        memset(&player.inventory[i].name, 0, GENERAL_LENGHT - 1);
        player.inventory[i].id = 0;
        player.inventory[i].x = 0;
        player.inventory[i].y = 0;
        player.inventory[i].glyph = glyph_blank;
      }
    }
  }

  if((player.inventory_item_selected - 1) >= 1)
  {
    player.inventory_item_selected--;
  }
}

internal void
add_inventory_item(item_t item)
{
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    if(!player.inventory[i].in_inventory)
    {
      player.inventory[i] = item;
      player.inventory[i].active = false;
      player.inventory[i].in_inventory = true;
      return;
    }
  }
}

internal i32
get_next_free_item_id()
{
  i32 next_free = 0;

  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    if(items[i].id > next_free)
    {
      next_free = items[i].id;
    }
  }

  if(next_free == 0)
  {
    return 1;
  }
  else
  {
    return next_free + 1;
  }
}

internal i32
add_item(i32 x, i32 y, item_e type)
{
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    if(!items[i].active && !items[i].in_inventory)
    {
      items[i].active = true;
      items[i].type = type;
      get_item_name_for_item_type(items[i].name, type);
      items[i].id = get_next_free_item_id();
      items[i].x = x;
      items[i].y = y;
      items[i].glyph = get_item_glyph_for_item_type(type);
      return items[i].id;
    }
  }

  return -1;
}

internal void
render_room()
{
  for(i32 x = 0; x < ROOM_WIDTH; x++)
  {
    for(i32 y = 0; y < ROOM_HEIGHT; y++)
    {
      char c[2] = {0};
      c[0] = room[x][y];

      i32 pair = white_pair;

      if(room[x][y] == glyph_stone ||
         room[x][y] == glyph_floor)
      {
        attron(COLOR_PAIR(stone_pair));
        pair = stone_pair;
      }
      else if(room[x][y] == glyph_bookshelf ||
              room[x][y] == glyph_crate ||
              room[x][y] == glyph_small_crate ||
              room[x][y] == glyph_table ||
              room[x][y] == glyph_chair ||
              room[x][y] == glyph_open_chest ||
              room[x][y] == glyph_wooden_door ||
              room[x][y] == glyph_wooden_door_open)
      {
        attron(COLOR_PAIR(wood_pair));
        pair = wood_pair;
      }
      else if(room[x][y] == glyph_stone_door ||
              room[x][y] == glyph_stone_door_open ||
              room[x][y] == glyph_chain)
      {
        attron(COLOR_PAIR(metal_pair));
        pair = metal_pair;
      }
      else if(room[x][y] == glyph_torch)
      {
        attron(COLOR_PAIR(yellow_pair));
        pair = yellow_pair;
      }

      mvprintw(y, x, c);
      attroff(COLOR_PAIR(pair));
    }
  }
}

internal void
render_player()
{
  attron(COLOR_PAIR(cyan_pair));
  mvprintw(player.y, player.x, "@");
  attroff(COLOR_PAIR(cyan_pair));
}

internal i32
is_searchable(i32 x, i32 y)
{
  for(i32 i = 0; i < SEARCHABLE_COUNT; i++)
  {
    if(equal_pos(x, y, searchables[i].x, searchables[i].y))
    {
      if(searchables[i].searched)
      {
        return 2;
      }
      else
      {
        return 1;
      }
    }
  }

  return 0;
}

internal void
push_loot_message(char **found_loot_names)
{
  i32 names_to_append = 0;
  for(i32 i = 0; i < LOOT_COUNT; i++)
  {
    if(*found_loot_names[i] != glyph_blank)
    {
      names_to_append++;
    }
  }

  if(names_to_append == 1)
  {
    render_message("You start searching..\n  you find something:\n  %s.", found_loot_names[0]);
  }
  else if(names_to_append == 2)
  {
    render_message("You start searching..\n  you find a couple things:\n  %s,\n  %s.", found_loot_names[0], found_loot_names[1]);
  }
  else if(names_to_append == 3)
  {
    render_message("You start searching..\n  you find multiple things:\n  %s,\n  %s,\n  %s.", found_loot_names[0], found_loot_names[1], found_loot_names[2]);
  }
}

internal void
add_searchable_loot(i32 x, i32 y)
{
  for(i32 i = 0; i < SEARCHABLE_COUNT; i++)
  {
    if(equal_pos(x, y, searchables[i].x, searchables[i].y))
    {
      char *found_loot_names[LOOT_COUNT];
      for(i32 i = 0; i < LOOT_COUNT; i++)
      {
        found_loot_names[i] = malloc(sizeof(char) * GENERAL_LENGHT);
        *found_loot_names[i] = glyph_blank;
      }

      for(i32 loot_index = 0; loot_index < LOOT_COUNT; loot_index++)
      {
        if(searchables[i].loot[loot_index])
        {
          get_item_name_for_item_type(found_loot_names[loot_index], searchables[i].loot[loot_index]);

          // Add item to game, make sure to mark it as not active and in the inventory
          // since it will be added to the inventory by default.
          i32 item_id = add_item(0, 0, searchables[i].loot[loot_index]);

          // Add item to inventory.
          i32 index = get_item_pos_for_id(item_id);
          items[index].active = false;
          items[index].in_inventory = true;
          add_inventory_item(items[index]);
        }
      }

      push_loot_message(found_loot_names);

      for(i32 i = 0; i < LOOT_COUNT; i++)
      {
        free(found_loot_names[i]);
      }

      searchables[i].searched = true;
      return;
    }
  }
}

internal void
pick_up(i32 x, i32 y)
{
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    if(items[i].active)
    {
      if(equal_pos(x, y, items[i].x, items[i].y))
      {
        render_message("You pick up the %s.", items[i].name);
        add_inventory_item(items[i]);
        items[i].active = false;
        items[i].in_inventory = true;
        return;
      }
    }
  }

  switch(room[x][y])
  {
    case glyph_stone: render_message("Perhaps if I were Hercules.."); break;
    case glyph_floor: render_message("There's nothing there to pick up."); break;
    case glyph_bookshelf: render_message("It would probably break and collapse on me if I moved it too much."); break;
    case glyph_crate: render_message("Might be a good workout but hardly a priority."); break;
    case glyph_small_crate: render_message("Might be a good workout but hardly a priority."); break;
    case glyph_stone_door: render_message("If only it was that simple.."); break;
    case glyph_wooden_door: render_message("If only it was that simple.."); break;
    case glyph_open_chest: render_message("Might be a good workout but hardly a priority."); break;
    default: render_message("You don't see any reason to pick that up.");
  }
}

internal void
use_item(i32 x, i32 y)
{
  render_message("What item do you want to use? (enter inventory character)");
  i32 input = getch() - ASCII_LOWERCASE_START;
  clear_message();

  if(input >= 0 && input <= ITEM_COUNT)
  {
    if(player.inventory[input - 1].in_inventory)
    {
      i32 type = get_item_type_for_inventory_position(input);

      if(room[x][y] == glyph_stone_door ||
         room[x][y] == glyph_stone_door_open)
      {
        if(game.first_door_spade_inserted)
        {
          if(game.first_door_cupric_sulfate_added)
          {
            if(game.first_door_dihydrogen_monoxide_added)
            {
              render_message("You don't have a reason to do that.");
            }
            else
            {
              if(type == item_dihydrogen_monoxide)
              {
                render_message("You pour the dihydrogen monoxide onto the cupric sulfate..\n  There's a reaction, you step back..\n  The spade gets hotter and expands a little.");
                remove_inventory_item(input);
                game.first_door_dihydrogen_monoxide_added = true;
                player.x--;
              }
              else
              {
                render_message("You don't have a reason to do that.");
              }
            }
          }
          else
          {
            if(type == item_cupric_sulfate)
            {
              render_message("You pour the cupric sulfate onto the flat part of the spade.");
              remove_inventory_item(input);
              game.first_door_cupric_sulfate_added = true;
            }
            else
            {
              render_message("You don't have a reason to do that.");
            }
          }
        }
        else
        {
          if(type == item_metal_spade_no_handle)
          {
            render_message("You push the other end of the spade in the hole..\n  It fits quite nicely.");
            remove_inventory_item(input);
            game.first_door_spade_inserted = true;
          }
          else
          {
            render_message("You don't have a reason to do that.");
          }
        }
      }
      else if(room[x][y] == glyph_chain)
      {
        if(game.second_door_key_imprint_made)
        {
          if(type == item_tin)
          {
            render_message("You already made an impri32 of the key.");
          }
          else
          {
            render_message("You don't have a reason to do that.");
          }
        }
        else
        {
          if(type == item_tin &&
             game.second_door_dihydrogen_monoxide_added &&
             game.second_door_gypsum_added)
          {
            render_message("You make an impression of the key in the white mixture as it hardens.");
            game.second_door_key_imprint_made = true;
          }
          else
          {
            render_message("You don't have a reason to do that.");
          }
        }
      }
      else if(room[x][y] == glyph_wooden_door)
      {
        if(game.second_door_key_inserted)
        {
          render_message("Nothing interesting happens.");
        }
        else
        {
          if(type == item_bronze_key && game.second_door_key_pried)
          {
            render_message("You insert the duplicate key and twist it..\n  You hear a loud click and the door is unlocked.");
            remove_inventory_item(input);
            game.second_door_key_inserted = true;
          }
          else
          {
            render_message("Nothing interesting happens.");
          }
        }
      }
    }
    else
    {
      render_message("There's nothing to use in that inventory slot.");
    }
  }
  else
  {
    render_message("There's no such inventory slot.");
  }
}

internal void
interact(i32 x, i32 y)
{
  i32 result = is_searchable(x, y);
  if(result == 1)
  {
    add_searchable_loot(x, y);
    return;
  }
  else if(result == 2)
  {
    switch(room[x][y])
    {
      case glyph_bookshelf: render_message("You search the crate again..\n  You don't find anything interesting."); break;
      case glyph_crate: render_message("You search the crate again..\n  You don't find anything interesting."); break;
    }

    return;
  }

  if(room[x][y] == glyph_stone_door)
  {
    if(game.first_door_dihydrogen_monoxide_added)
    {
      render_message("You pull on the spade..\n  It doesn't seem to budge so you pull hard on it..\n  The door slowly opens!");
      player.x--;

      room[20][4] = glyph_stone_door_open;
      room[21][4] = glyph_floor;

      game.first_door_open = true;
    }
    else
    {
      if(game.first_door_cupric_sulfate_added)
      {
        render_message("Probably shouldn't move the spade because of the ingrients on it.");
      }
      else
      {
        if(game.first_door_spade_inserted)
        {
          render_message("You try to open the door using the spade as leverage..\n  The spade falls out since there's nothing actually holding it in place.");
        }
        else
        {
          render_message("The door won't budge.");
        }
      }
    }

    return;
  }
  else if(room[x][y] == glyph_wooden_door)
  {
    if(game.second_door_key_inserted)
    {
      render_message("You twist the bronze key in the lock..\n  The door becomes unlocked and you open it.");

      room[23][4] = glyph_wooden_door_open;

      game.second_door_open = true;
    }
    else
    {
      render_message("The door won't budge.");
    }

    return;
  }

  switch(room[x][y])
  {
    case glyph_bookshelf: render_message("You search the bookshelf..\n  you find nothing useful."); break;
    case glyph_crate: render_message("You search the crate..\n  you find nothing useful."); break;
    case glyph_small_crate: render_message("You search the small crate..\n  you find nothing useful."); break;
    case glyph_open_chest: render_message("There's nothing in there."); break;
    case glyph_table: render_message("You look under the table..\n  nothing but rocks, dust and worms crawling around."); break;
    case glyph_chair: render_message("Just an ordinary chair, except it looks old as hell and beat up."); break;
    case glyph_chain: render_message("You don't see a way of getting the key because of the chain."); break;
    case glyph_stone_door_open: render_message("You already opened it."); break;
    case glyph_wooden_door_open: render_message("You already opened it."); break;
    default: render_message("You don't see anything to do here.");
  }
}

internal void
inspect(i32 x, i32 y)
{
  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    if(items[i].active)
    {
      if(equal_pos(x, y, items[i].x, items[i].y))
      {
        switch(items[i].glyph)
        {
          case glyph_metal_spade: render_message("A metal spade, it's got a wooden handle to it."); break;
          case glyph_metal_spade_no_handle: render_message("A metal spade, it has no handle to it."); break;
          case glyph_knife: render_message("A rugged looking knife, I wonder what I could do with this."); break;
          case glyph_magnet: render_message("A curved magnet."); break;
          case glyph_bunsen_burner: render_message("A bunsen burner, good for combusting things."); break;
          case glyph_bronze_key: render_message("A bronze key, still a little warm."); break;
          case glyph_tin:
          {
            if(game.second_door_key_complete)
            {
              render_message("A round container made out of tin..\n  There's a bronze key in the imprint.");
            }
            else if(game.second_door_cupric_ore_powder_added && game.second_door_tin_ore_powder_added)
            {
              render_message("A round container made out of tin..\n  The key impri32 has cupric and tin ore powder in it.");
            }
            else if(game.second_door_cupric_ore_powder_added)
            {
              render_message("A round container made out of tin..\n  The key impri32 has cupric ore powder in it.");
            }
            else if(game.second_door_tin_ore_powder_added)
            {
              render_message("A round container made out of tin..\n  The key impri32 has tin ore powder in it.");
            }
            else if(game.second_door_key_imprint_made)
            {
              render_message("A round container made out of tin..\n  It's filled with a lumpy white mixture that has an impri32 of a key.");
            }
            else if(game.second_door_gypsum_added && game.second_door_dihydrogen_monoxide_added)
            {
              render_message("A round container made out of tin..\n  It's filled with a lumpy white mixture.");
            }
            else if(game.second_door_gypsum_added)
            {
              render_message("A round container made out of tin..\n  It has gypsum in it.");
            }
            else if(game.second_door_dihydrogen_monoxide_added)
            {
              render_message("A round container made out of tin..\n  It has dihydrogen monoxide in it.");
            }
            else
            {
              render_message("A round container made out of tin."); break;
            }
          } break;
          case glyph_vial:
          {
            item_e type = get_item_type_for_pos(x, y);
            if(type == item_empty_vial)
            {
              render_message("It's a glass vial but it's empty.");
            }
            else if(type == item_dihydrogen_monoxide)
            {
              render_message("A vial filled with clear blue liquid.\n  It has a label that says \"Dihydrogen Monoxide\".");
            }
            else if(type == item_cupric_ore_powder)
            {
              render_message("A vial filled with orange liquid.\n  It has a label that says \"Powdered Cupric Ore\".");
            }
            else if(type == item_tin_ore_powder)
            {
              render_message("A vial filled with dark liquid.\n  It has a label that says \"Powdered Tin Ore\".");
            }
            else if(type == item_sodium_chloride)
            {
              render_message("A vial filled with white substance.\n  It has a label that says \"Sodium Chloride\".");
            }
            else if(type == item_gypsum)
            {
              render_message("A vial filled with gray liquid.\n  It has a label that says \"Gypsum\".");
            }
            else if(type == item_cupric_sulfate)
            {
              render_message("A vial filled with white substance.\n  It has a label that says \"Cupric Sulfate\".");
            }
            else if(type == item_acetic_acid)
            {
              render_message("A vial filled with liquid that's dark green.\n  It has a label that says \"Acetic Acid\".");
            }
          } break;
        }

        return;
      }
    }
  }

  switch(room[x][y])
  {
    case glyph_stone: render_message("A stone surface, looks old and covered in moss."); break;
    case glyph_floor: render_message("An uneven stone floor."); break;
    case glyph_bookshelf: render_message("A tall old bookshelf with some haphazardly placed books in it."); break;
    case glyph_crate: render_message("A large wooden crate."); break;
    case glyph_small_crate: render_message("A small wooden crate."); break;
    case glyph_open_chest: render_message("A wooden chest that's already open, it's completely empty."); break;
    case glyph_table: render_message("A worn down rickety table with text and markings all over it."); break;
    case glyph_chair: render_message("A chair exactly like the other ones in this room..\n  Some are missing their legs."); break;
    case glyph_torch: render_message("A torch on the wall, it burns very dimly."); break;
    case glyph_chain: render_message("A stone surface with a big chain hanging from it all the way down to the ground..\n  There's a bronze key at the end of the chain."); break;
    case glyph_stone_door_open: render_message("It's the stone door but it's wide open this time."); break;
    case glyph_wooden_door_open: render_message("It's the wooden door but it's wide open this time."); break;
    case glyph_wooden_door:
    {
      if(game.second_door_key_inserted)
      {
        render_message("A door made out of wood..\n  It's got a bronze key inserted.");
      }
      else
      {
        render_message("A door made out of wood..\n  It's got a lock on it with a keyhole.");
      }
    } break;
    case glyph_stone_door:
    {
      if(game.first_door_spade_inserted &&
         game.first_door_cupric_sulfate_added &&
         game.first_door_dihydrogen_monoxide_added)
      {
        render_message("The spade is warm and has slightly expanded.");
      }
      else if(game.first_door_spade_inserted && game.first_door_cupric_sulfate_added)
      {
        render_message("The spade has cupric sulfate on it.");
      }
      else if(game.first_door_spade_inserted)
      {
        render_message("The spade is sticking out of the hole in the door.");
      }
      else
      {
        render_message("A door but it's thick and made out of stone!\n  It seems to have a hole in it that doesn't fully go through.");
      }
    } break;
  }
}

internal void
reset_inventory_selections()
{
  player.inventory_first_combination_item_num = 0;
  player.inventory_second_combination_item_num = 0;
  player.inventory_first_combination_item = item_none;
  player.inventory_second_combination_item = item_none;
  player.inventory_first_combination_item = item_none;
  player.inventory_second_combination_item = item_none;
}

internal void
combine(item_e first_type, item_e second_type)
{
  char first_name[GENERAL_LENGHT];
  char second_name[GENERAL_LENGHT];

  get_item_name_for_item_type(first_name, first_type);
  get_item_name_for_item_type(second_name, second_type);

  if((first_type == item_metal_spade && second_type == item_bunsen_burner) ||
     (first_type == item_bunsen_burner && second_type == item_metal_spade))
  {
    render_message("You use the bunsen burner to burn the handle away from the spade..\n  You are left with a metal spade that has no handle.", first_name, second_name);

    if(first_type == item_metal_spade)
    {
      remove_inventory_item(player.inventory_first_combination_item_num);
    }
    else
    {
      remove_inventory_item(player.inventory_second_combination_item_num);
    }

    // Add item to game, make sure to mark it as not active and in the inventory
    // since it will be added to the inventory by default.
    i32 item_id = add_item(0, 0, item_metal_spade_no_handle);

    // Add item to inventory.
    i32 index = get_item_pos_for_id(item_id);
    items[index].active = false;
    items[index].in_inventory = true;
    add_inventory_item(items[index]);
  }
  else if((first_type == item_tin && second_type == item_dihydrogen_monoxide) ||
          (first_type == item_dihydrogen_monoxide && second_type == item_tin))
  {
    if(game.second_door_dihydrogen_monoxide_added)
    {
      render_message("There's already some dihydrogen monoxide in the tin.");
    }
    else
    {
      if(game.second_door_gypsum_added)
      {
        render_message("You pour the dihydrogen monoxide in the tin..\n  The result is a lumpy white mixture.");
      }
      else
      {
        render_message("You pour the dihydrogen monoxide in the tin..");
      }

      if(first_type == item_dihydrogen_monoxide)
      {
        remove_inventory_item(player.inventory_first_combination_item_num);
      }
      else
      {
        remove_inventory_item(player.inventory_second_combination_item_num);
      }

      game.second_door_dihydrogen_monoxide_added = true;
    }
  }
  else if((first_type == item_tin && second_type == item_gypsum) ||
          (first_type == item_gypsum && second_type == item_tin))
  {
    if(game.second_door_dihydrogen_monoxide_added)
    {
      render_message("You pour the gypsum in the tin..\n  The result is a lumpy white mixture.");
    }
    else
    {
      render_message("You pour the gypsum in the tin.");
    }

    if(first_type == item_gypsum)
    {
      remove_inventory_item(player.inventory_first_combination_item_num);
    }
    else
    {
      remove_inventory_item(player.inventory_second_combination_item_num);
    }

    game.second_door_gypsum_added = true;
  }
  else if((first_type == item_tin && second_type == item_cupric_ore_powder) ||
          (first_type == item_cupric_ore_powder && second_type == item_tin))
  {
    if(game.second_door_key_imprint_made)
    {
      if(first_type == item_cupric_ore_powder)
      {
        remove_inventory_item(player.inventory_first_combination_item_num);
      }
      else
      {
        remove_inventory_item(player.inventory_second_combination_item_num);
      }

      render_message("You pour the cupric ore powder into the impression of the key.");
      game.second_door_cupric_ore_powder_added = true;
    }
    else
    {
      render_message("Nothing interesting happens.");
    }
  }
  else if((first_type == item_tin && second_type == item_tin_ore_powder) ||
          (first_type == item_tin_ore_powder && second_type == item_tin))
  {
    if(game.second_door_key_imprint_made)
    {
      if(first_type == item_tin_ore_powder)
      {
        remove_inventory_item(player.inventory_first_combination_item_num);
      }
      else
      {
        remove_inventory_item(player.inventory_second_combination_item_num);
      }

      render_message("You pour the tin ore powder into the impression of the key.");
      game.second_door_tin_ore_powder_added = true;
    }
    else
    {
      render_message("Nothing interesting happens.");
    }
  }
  else if((first_type == item_tin && second_type == item_bunsen_burner) ||
          (first_type == item_bunsen_burner && second_type == item_tin))
  {
    if(game.second_door_cupric_ore_powder_added &&
       game.second_door_tin_ore_powder_added)
    {
      render_message("You heat the two powdered ores together in the tin..\n  You make a duplicate of the key in bronze.\n  Now you need to pry it out of the tin somehow.");
      game.second_door_key_complete = true;
    }
    else
    {
      render_message("Nothing interesting happens.");
    }
  }
  else if((first_type == item_knife && second_type == item_tin) ||
          (first_type == item_tin && second_type == item_knife))
  {
    if(game.second_door_key_complete)
    {
      render_message("You pry the duplicate bronze key out of the tin.");

      // Add item to game, make sure to mark it as not active and in the inventory
      // since it will be added to the inventory by default.
      i32 item_id = add_item(0, 0, item_bronze_key);

      // Add item to inventory.
      i32 index = get_item_pos_for_id(item_id);
      items[index].active = false;
      items[index].in_inventory = true;
      add_inventory_item(items[index]);

      game.second_door_key_pried = true;
    }
    else
    {
      render_message("Nothing interesting happens.");
    }
  }
  else
  {
    render_message("Nothing interesting happens.");
  }

  reset_inventory_selections();
}

internal void
player_keypress(i32 key)
{
  i32 player_new_x = player.x;
  i32 player_new_y = player.y;

  if(player.inventory_enabled)
  {
    if(key == 'b')
    {
      player.inventory_enabled = false;
      player.inventory_item_selected = 0;
      reset_inventory_selections();
    }
    else if(key == 'w')
    {
      if((player.inventory_item_selected - 1) < 1)
      {
        player.inventory_item_selected = player.inventory_item_count;
      }
      else
      {
        player.inventory_item_selected--;
      }
    }
    else if(key == 's')
    {
      if((player.inventory_item_selected + 1) > player.inventory_item_count)
      {
        player.inventory_item_selected = 1;
      }
      else
      {
        player.inventory_item_selected++;
      }
    }
    else if(key == 'd')
    {
      drop_inventory_item(player_new_x, player_new_y, player.inventory_item_selected);
    }
    else if(key == 'c')
    {
      if(player.inventory_first_combination_item == item_none)
      {
        player.inventory_first_combination_item_num = player.inventory_item_selected;
        player.inventory_first_combination_item = get_item_type_for_inventory_position(player.inventory_item_selected);
      }
      else if(player.inventory_second_combination_item == item_none)
      {
        player.inventory_second_combination_item_num = player.inventory_item_selected;

        if(player.inventory_first_combination_item_num == player.inventory_second_combination_item_num)
        {
          render_message("Nothing interesting happens.");
          reset_inventory_selections();
        }
        else
        {
          player.inventory_second_combination_item = get_item_type_for_inventory_position(player.inventory_item_selected);
          combine(player.inventory_first_combination_item, player.inventory_second_combination_item);
        }
      }
    }
  }

  else if(player.using_an_item)
  {
    if(key == 'w')
    {
      player_new_y--;
    }
    else if(key == 's')
    {
      player_new_y++;
    }
    else if(key == 'a')
    {
      player_new_x--;
    }
    else if(key == 'd')
    {
      player_new_x++;
    }

    use_item(player_new_x, player_new_y);
    player.using_an_item = false;
  }
  else if(player.picking_up)
  {
    if(key == 'w')
    {
      player_new_y--;
    }
    else if(key == 's')
    {
      player_new_y++;
    }
    else if(key == 'a')
    {
      player_new_x--;
    }
    else if(key == 'd')
    {
      player_new_x++;
    }

    pick_up(player_new_x, player_new_y);
    player.picking_up = false;
  }
  else if(player.interacting)
  {
    if(key == 'w')
    {
      player_new_y--;
    }
    else if(key == 's')
    {
      player_new_y++;
    }
    else if(key == 'a')
    {
      player_new_x--;
    }
    else if(key == 'd')
    {
      player_new_x++;
    }

    interact(player_new_x, player_new_y);
    player.interacting = false;
  }
  else if(player.inspecting)
  {
    if(key == 'w')
    {
      player_new_y--;
    }
    else if(key == 's')
    {
      player_new_y++;
    }
    else if(key == 'a')
    {
      player_new_x--;
    }
    else if(key == 'd')
    {
      player_new_x++;
    }

    inspect(player_new_x, player_new_y);
    player.inspecting = false;
  }
  else
  {
    if(key == 'w')
    {
      player_new_y--;
    }
    else if(key == 's')
    {
      player_new_y++;
    }
    else if(key == 'a')
    {
      player_new_x--;
    }
    else if(key == 'd')
    {
      player_new_x++;
    }
    else if(key == 'u')
    {
      if(player.inventory_item_count)
      {
        render_message("Where do you want to use the item?");
        player.using_an_item = 1;
      }
      else
      {
        render_message("You don't have anything to use.");
      }
    }
    else if(key == 'i')
    {
      render_message("What do you want to interact with?");
      player.interacting = true;
    }
    else if(key == 'o')
    {
      render_message("What do you want to inspect?");
      player.inspecting = true;
    }
    else if(key == 'p')
    {
      render_message("What do you want to pickup?");
      player.picking_up = true;
    }
    else if(key == 'b')
    {
      player.inventory_enabled = true;
      player.inventory_item_selected = 1;
    }

    if(is_traversable(player_new_x, player_new_y))
    {
      player.x = player_new_x;
      player.y = player_new_y;
    }

    player.turn++;
  }
}

internal void
update_input()
{
  player.input = getch();
  clear_message();

  if(is_valid_input(player.input))
  {
    if(player.input == 'q' ||
       player.input == 'Q')
    {
      game.state = state_quit;
    }
    else
    {
      player_keypress(player.input);
    }
  }
}

internal void
render_ui()
{
  mvprintw(10, 0, "%s", player.name);
  // NOTE(Rami): Add things that affect HP later
  mvprintw(12, 0, "HP: %d", player.hp);
  mvprintw(13, 0, "Turn: %d", player.turn);

  #if REBIRTH_SLOW
  i32 x = 135;
  i32 y = 1;
  i32 num = 0;
  for(i32 i = 0; i < pair_count; i++)
  {
    attron(COLOR_PAIR(i));
    mvprintw(y, x, "#");
    attroff(COLOR_PAIR(i));

    x++;
    num++;

    if(num == 16)
    {
      x = 135;
      y++;
      num = 0;
    }
  }

  i32 debug_x = 0;
  i32 debug_y = 22;

  mvprintw(debug_y, debug_x, "player x: %d\n", player.x);
  mvprintw(debug_y + 1, debug_x, "player y: %d\n", player.y);

  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    mvprintw(debug_y + 3, debug_x, "active %d\n", items[i].active);
    mvprintw(debug_y + 4, debug_x, "type %d\n", items[i].type);
    mvprintw(debug_y + 5, debug_x, "in_inventory %d\n", items[i].in_inventory);
    mvprintw(debug_y + 6, debug_x, "name %s\n", items[i].name);
    mvprintw(debug_y + 7, debug_x, "id %d\n", items[i].id);
    mvprintw(debug_y + 8, debug_x, "x %d\n", items[i].x);
    mvprintw(debug_y + 9, debug_x, "y %d\n", items[i].y);
    mvprintw(debug_y + 10, debug_x, "glyph %c\n", items[i].glyph);

    debug_y = debug_y + 9;
  }

  debug_y = 22;
  debug_x = 60;

  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    mvprintw(debug_y, debug_x, "active %d\n", player.inventory[i].active);
    mvprintw(debug_y + 1, debug_x, "type %d\n", player.inventory[i].type);
    mvprintw(debug_y + 2, debug_x, "in_inventory %d\n", player.inventory[i].in_inventory);
    mvprintw(debug_y + 3, debug_x, "name %s\n", player.inventory[i].name);
    mvprintw(debug_y + 4, debug_x, "id %d\n", player.inventory[i].id);
    mvprintw(debug_y + 5, debug_x, "x %d\n", player.inventory[i].x);
    mvprintw(debug_y + 6, debug_x, "y %d\n", player.inventory[i].y);
    mvprintw(debug_y + 7, debug_x, "glyph %c\n", player.inventory[i].glyph);

    debug_y = debug_y + 9;
  }

  mvprintw(1, 86, "first_door_open: %d", game.first_door_open);
  mvprintw(2, 86, "first_door_dihydrogen_monoxide_added: %d", game.first_door_dihydrogen_monoxide_added);
  mvprintw(3, 86, "first_door_cupric_sulfate_added: %d", game.first_door_cupric_sulfate_added);
  mvprintw(4, 86, "first_door_spade_inserted: %d", game.first_door_spade_inserted);

  mvprintw(6, 86, "second_door_open: %d", game.second_door_open);
  mvprintw(7, 86, "second_door_key_pried: %d", game.second_door_key_pried);
  mvprintw(8, 86, "second_door_key_complete: %d", game.second_door_key_complete);
  mvprintw(9, 86, "second_door_tin_ore_powder_added: %d", game.second_door_tin_ore_powder_added);
  mvprintw(10, 86, "second_door_cupric_ore_powder_added: %d", game.second_door_cupric_ore_powder_added);
  mvprintw(11, 86, "second_door_key_imprint_made: %d", game.second_door_key_imprint_made);
  mvprintw(12, 86, "second_door_gypsum_added: %d", game.second_door_gypsum_added);
  mvprintw(13, 86, "second_door_dihydrogen_monoxide_added: %d", game.second_door_dihydrogen_monoxide_added);
  #endif
}

internal void
render_inventory()
{
  mvprintw(0, 49, "Inventory");

  mvhline(1, 26, ACS_HLINE, 83 - 26);
  mvhline(12, 26, ACS_HLINE, 83 - 26);
  mvvline(1, 26, ACS_VLINE, 12 - 1);
  mvvline(1, 83, ACS_VLINE, 12 - 1);
  mvaddch(1, 26, ACS_ULCORNER);
  mvaddch(12, 26, ACS_LLCORNER);
  mvaddch(1, 83, ACS_URCORNER);
  mvaddch(12, 83, ACS_LRCORNER);

  i32 count = 0;
  i32 start_x = 28;
  i32 start_y = 2;
  i32 x = start_x;
  i32 y = start_y;

  char clear[28] = {0};
  memset(clear, ' ', sizeof(clear) - 1);

  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    mvprintw(y, x, clear);
    y++;

    if(y > 11)
    {
      x += 28;
      y = 2;
    }
  }

  x = start_x;
  y = start_y;

  for(i32 i = 0; i < ITEM_COUNT; i++)
  {
    if(player.inventory[i].in_inventory)
    {
      count++;

      if(count == player.inventory_item_selected)
      {
        attron(COLOR_PAIR(cyan_pair));
        mvprintw(y, x, "%c: %s", 96 + count, player.inventory[i].name);        
        attroff(COLOR_PAIR(cyan_pair));
      }
      else if(count == player.inventory_first_combination_item_num ||
              count == player.inventory_second_combination_item_num)
      {
        attron(COLOR_PAIR(grey_pair));
        mvprintw(y, x, "%c: %s", 96 + count, player.inventory[i].name);
        attroff(COLOR_PAIR(grey_pair));
      }
      else
      {
        mvprintw(y, x, "%c: %s", 96 + count, player.inventory[i].name);
      }

      y++;

      if(y > 11)
      {
        x += 28;
        y = 2;
      }
    }
  }

  player.inventory_item_count = count;
  if(!player.inventory_item_count)
  {
    player.inventory_enabled = false;
    player.inventory_item_selected = 0;
    reset_inventory_selections();
  }
}

internal void
run_game()
{
  while(game.state != state_quit)
  {
    if(game.state == state_main_menu)
    {
      render_main_menu();
      update_main_menu();
    }
    else if(game.state == state_play)
    {
      render_room();
      render_items();
      render_player();
      render_ui();
      render_inventory();

      update_input();
    }
  }
}

internal void
add_searchable(i32 x, i32 y, item_e item_one, item_e item_two, item_e item_three)
{
  for(i32 i = 0; i < SEARCHABLE_COUNT; i++)
  {
    if(equal_pos(0, 0, searchables[i].x, searchables[i].y))
    {
      searchables[i].x = x;
      searchables[i].y = y;
      searchables[i].loot[0] = item_one;
      searchables[i].loot[1] = item_two;
      searchables[i].loot[2] = item_three;
      break;
    }
  }
}

internal i32
init_game()
{
  initscr();

  // NOTE(Rami): Maybe have an enum for the possible error numbers,
  // then pass that to exit_game() for example so it can print out
  // the correct error after ncurses has been closed.

  // NOTE(Rami): Add && to check COLORS is at least the amount of colors we need.
  if(!has_colors())
  {
    printf("Your terminal does not support colors.\n");
    return 0;
  }

  start_color();
  curs_set(0);        // enable/disable cursor
  keypad(stdscr, 1);  // enable/disable F1, F2, arrow keys etc.
  noecho();           // getch character will not be printed on screen
  nodelay(stdscr, 0); // will getch block execution
  cbreak();           // getch will return user input immediately

  init_color(COLOR_BLACK, 0, 0, 0);
  init_color(COLOR_RED, 1000, 0, 0);
  init_color(COLOR_GREEN, 0, 1000, 0);
  init_color(COLOR_YELLOW, 1000, 1000, 0);
  init_color(COLOR_BLUE, 0, 0, 1000);
  init_color(COLOR_MAGENTA, 1000, 0, 1000);
  init_color(COLOR_CYAN, 0, 1000, 1000);
  init_color(COLOR_WHITE, 1000, 1000, 1000);
  init_color(color_stone, 545, 640, 545);
  init_color(color_wood, 627, 321, 176);
  init_color(color_metal, 780, 780, 780);
  init_color(color_grey, 200, 200, 200);

  init_pair(black_pair, COLOR_BLACK, COLOR_BLACK);
  init_pair(red_pair, COLOR_RED, COLOR_BLACK);
  init_pair(green_pair, COLOR_GREEN, COLOR_BLACK);
  init_pair(yellow_pair, COLOR_YELLOW, COLOR_BLACK);
  init_pair(blue_pair, COLOR_BLUE, COLOR_BLACK);
  init_pair(magenta_pair, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(cyan_pair, COLOR_CYAN, COLOR_BLACK);
  init_pair(white_pair, COLOR_WHITE, COLOR_BLACK);
  init_pair(stone_pair, color_stone, COLOR_BLACK);
  init_pair(wood_pair, color_wood, COLOR_BLACK);
  init_pair(metal_pair, color_metal, COLOR_BLACK);
  init_pair(grey_pair, color_grey, COLOR_BLACK);
  init_pair(white_on_grey_pair, COLOR_WHITE, color_grey);

  init_pair(background_grey_pair, color_grey, color_grey);

  // Stone
  for(i32 x = 0; x < ROOM_WIDTH; x++)
  {
    for(i32 y = 0; y < ROOM_HEIGHT; y++)
    {
      room[x][y] = glyph_stone;
    }
  }

  // Floor
  for(i32 x = 3; x < ROOM_WIDTH - 3; x++)
  {
    for(i32 y = 2; y < ROOM_HEIGHT - 2; y++)
    {
      room[x][y] = glyph_floor;
    }
  }

  room[2][4] = glyph_floor;
  room[7][1] = glyph_floor;
  room[8][1] = glyph_floor;
  room[9][1] = glyph_floor;
  room[13][1] = glyph_floor;
  room[14][1] = glyph_floor;
  room[15][1] = glyph_floor;
  room[16][1] = glyph_floor;
  room[6][8] = glyph_floor;
  room[7][8] = glyph_floor;
  room[8][8] = glyph_floor;
  room[9][8] = glyph_floor;
  room[10][8] = glyph_floor;
  room[11][8] = glyph_floor;
  room[12][8] = glyph_floor;
  room[13][8] = glyph_floor;
  room[20][4] = glyph_floor;
  room[21][4] = glyph_floor;
  room[22][4] = glyph_floor;

  // Bookshelves
  room[7][1] = glyph_bookshelf;
  room[8][1] = glyph_bookshelf;
  room[9][1] = glyph_bookshelf;
  room[14][1] = glyph_bookshelf;
  room[3][2] = glyph_bookshelf;
  room[4][2] = glyph_bookshelf;
  room[4][7] = glyph_bookshelf;
  room[5][7] = glyph_bookshelf;

  room[7][8] = glyph_bookshelf;
  room[8][8] = glyph_bookshelf;
  room[9][8] = glyph_bookshelf;
  room[11][8] = glyph_bookshelf;

  // Crates
  room[19][2] = glyph_crate;
  room[20][2] = glyph_crate;
  room[20][6] = glyph_crate;
  room[19][7] = glyph_crate;
  room[20][7] = glyph_crate;

  // Small crates
  room[18][2] = glyph_small_crate;
  room[19][6] = glyph_small_crate;

  // Doors
  room[21][4] = glyph_stone_door;
  room[23][4] = glyph_wooden_door;

  // Chests
  room[20][3] = glyph_open_chest;

  // Tables
  room[10][4] = glyph_table;
  room[11][4] = glyph_table;
  room[12][4] = glyph_table;
  room[13][4] = glyph_table;
  room[10][5] = glyph_table;
  room[11][5] = glyph_table;
  room[12][5] = glyph_table;
  room[13][5] = glyph_table;

  // Chairs
  room[11][3] = glyph_chair;
  room[10][6] = glyph_chair;
  room[14][3] = glyph_chair;

  // Torches
  room[3][5] = glyph_torch;
  room[16][7] = glyph_torch;

  // Chains
  room[2][4] = glyph_chain;

  // NOTE(Rami): Skip main menu
  game.state = state_play;

  // Game
  game.menu_option_selected = 1;
  game.menu_option_count = 3;

  // Player
  strcpy(player.name, "Frozii");
  player.hp = 10;
  player.x = 3;
  player.y = 6;

  // Items
  add_item(13, 4, item_metal_spade);
  add_item(12, 5, item_bunsen_burner);
  add_item(10, 4, item_empty_vial);

  // Searchables
  add_searchable(4, 7, item_knife, item_none, item_none);
  add_searchable(7, 8, item_dihydrogen_monoxide, item_dihydrogen_monoxide, item_dihydrogen_monoxide);
  add_searchable(8, 8, item_cupric_ore_powder, item_none, item_none);
  add_searchable(9, 8, item_tin_ore_powder, item_none, item_none);
  add_searchable(11, 8, item_empty_vial, item_none, item_none);
  add_searchable(19, 2, item_tin, item_none, item_none);
  add_searchable(14, 1, item_sodium_chloride, item_none, item_none);
  add_searchable(9, 1, item_gypsum, item_none, item_none);
  add_searchable(8, 1, item_cupric_sulfate, item_none, item_none);
  add_searchable(7, 1, item_dihydrogen_monoxide, item_acetic_acid, item_none);
  add_searchable(3, 2, item_magnet, item_none, item_none);

  return 1;
}

i32
main()
{
  if(init_game())
  {
    run_game();
    endwin();
    return EXIT_SUCCESS;
  }

  endwin();
  return EXIT_FAILURE;
}