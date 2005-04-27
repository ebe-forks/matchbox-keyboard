#include "matchbox-keyboard.h"

MBKeyboard*
mb_kbd_new(int argc, char **argv)
{
  MBKeyboard *kb = NULL;
  char       *variant = NULL; 

  kb = util_malloc0(sizeof(MBKeyboard));

  kb->key_border = 1;
  kb->key_pad    = 2;
  kb->key_margin = 0;

  kb->col_spacing = 5;
  kb->row_spacing = 5;

  kb->font_family  = strdup("sans");
  kb->font_pt_size = 8;
  kb->font_variant = strdup("bold");

  if (argc > 1) 
    {
      variant = argv[1];
      DBG("varient is %s\n", variant);
    }
  else variant = getenv("MB_KBD_VARIANT");

  if (!mb_kbd_ui_init(kb))
    return NULL;

  if (mb_kbd_ui_display_width(kb->ui) <= 320) 
    {
      kb->key_pad      = 1;
      kb->col_spacing  = 2;
      kb->row_spacing  = 2;
      kb->font_pt_size = 5;
    }

  if (!mb_kbd_config_load(kb, variant))
    return NULL;

  kb->selected_layout 
    = (MBKeyboardLayout *)util_list_get_nth_data(kb->layouts, 0);

  if (!mb_kbd_ui_realize(kb->ui))
    return NULL;

  return kb;
}

int
mb_kbd_row_spacing(MBKeyboard *kb)
{
  return kb->row_spacing;
}

int
mb_kbd_col_spacing(MBKeyboard *kb)
{
  return kb->col_spacing;
}

int
mb_kbd_keys_border(MBKeyboard *kb)
{
  return kb->key_border;
}

int
mb_kbd_keys_pad(MBKeyboard *kb)
{
  return kb->key_pad;
}

int
mb_kbd_keys_margin(MBKeyboard *kb)
{
  return kb->key_margin;
}

void
mb_kbd_add_state(MBKeyboard *kbd, MBKeyboardStateType state)
{
  kbd->keys_state |= state;
}

void
mb_kbd_toggle_state(MBKeyboard *kbd, MBKeyboardStateType state)
{
  kbd->keys_state ^= state;
}

boolean
mb_kbd_has_state(MBKeyboard *kbd, MBKeyboardStateType state)
{
  return (kbd->keys_state & state);
}

boolean
mb_kbd_has_any_state(MBKeyboard *kbd)
{
  return  (kbd->keys_state > 0);
}

void
mb_kbd_remove_state(MBKeyboard *kbd, MBKeyboardStateType state)
{
  kbd->keys_state &= ~(state);
}

MBKeyboardKeyStateType
mb_kbd_keys_current_state(MBKeyboard *kbd)
{
  if (mb_kbd_has_state(kbd, MBKeyboardStateShifted))
    return MBKeyboardKeyStateShifted;

  if (mb_kbd_has_state(kbd, MBKeyboardStateMod1))
    return MBKeyboardKeyStateMod1;

  if (mb_kbd_has_state(kbd, MBKeyboardStateMod2))
    return MBKeyboardKeyStateMod2;
  
  if (mb_kbd_has_state(kbd, MBKeyboardStateMod3))
    return MBKeyboardKeyStateMod3;
  
  return MBKeyboardKeyStateNormal;
}

void
mb_kbd_redraw(MBKeyboard *kb)
{
  mb_kbd_ui_redraw(kb->ui);
}

void
mb_kbd_redraw_key(MBKeyboard *kb, MBKeyboardKey *key)
{
  mb_kbd_ui_redraw_key(kb->ui, key);
  mb_kbd_ui_swap_buffers(kb->ui);
}

MBKeyboardKey*
mb_kbd_locate_key(MBKeyboard *kb, int x, int y)
{
  MBKeyboardLayout *layout;
  List             *row_item, *key_item;

  layout = mb_kbd_get_selected_layout(kb);

  row_item = mb_kbd_layout_rows(layout);

  while (row_item != NULL)
    {
      MBKeyboardRow *row = row_item->data;

      if (x >= mb_kbd_row_x(row) 
	  && x <= mb_kbd_row_x(row) + mb_kbd_row_width(row) 
	  && y >= mb_kbd_row_y(row)
	  && y <= mb_kbd_row_y(row) + mb_kbd_row_height(row) )
	{
	  mb_kbd_row_for_each_key(row, key_item) 
	    {
	      MBKeyboardKey *key = key_item->data;

	      if (!mb_kbd_is_extended(kb) 
		      && mb_kbd_key_get_extended(key))
		continue;

	      if (!mb_kbd_key_is_blank(key)
		  && x >= mb_kbd_key_abs_x(key)
		  && x <= mb_kbd_key_abs_x(key) + mb_kbd_key_width(key))
		return key;

	    }
	 
	  return NULL;
	}

      row_item = util_list_next(row_item);
    }
  return NULL;
}

void
mb_kbd_add_layout(MBKeyboard *kb, MBKeyboardLayout *layout)
{
  kb->layouts = util_list_append(kb->layouts, (pointer)layout);
}

MBKeyboardLayout*
mb_kbd_get_selected_layout(MBKeyboard *kb)
{
  return kb->selected_layout;
}

void
mb_kbd_set_held_key(MBKeyboard *kb, MBKeyboardKey *key)
{
  kb->held_key = key;
}

MBKeyboardKey *
mb_kbd_get_held_key(MBKeyboard *kb)
{
  return kb->held_key;
}

void
mb_kbd_set_extended(MBKeyboard *kb, boolean extend)
{
  kb->extended = extend;
}

boolean
mb_kbd_is_extended(MBKeyboard *kb)
{
  return kb->extended;
}


void
mb_kbd_run(MBKeyboard *kb)
{
  /* mb_kbb_ui_process_events() */


}


int 
main(int argc, char **argv)
{
  MBKeyboard *kb;

  kb = mb_kbd_new(argc, argv);

  if (kb) mb_kbd_run(kb);

  return 0;
}
