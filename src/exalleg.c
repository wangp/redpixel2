/* export allegro functions and globals 
 * pinched from SeeR/examples/example6.c (too tedious and boring!)
 */

#include <allegro.h>
#include <seer.h>

void export_allegro()
{ 
    scAddExtSym(allegro_init);
    scAddExtSym(allegro_exit);
    scAddExtSym(install_mouse);
    scAddExtSym(install_timer);
    scAddExtSym(install_keyboard);
    scAddExtSym(screen);
    scAddExtSym(set_color_depth);
    scAddExtSym(set_color_conversion);
    scAddExtSym(set_gfx_mode);
    scAddExtSym(create_bitmap);
    scAddExtSym(create_bitmap_ex);
    scAddExtSym(create_sub_bitmap);
    //scAddExtSym(create_video_bitmap);
    scAddExtSym(destroy_bitmap);
    scAddExtSym(set_color);
    scAddExtSym(set_pallete);
    scAddExtSym(set_pallete_range);
    scAddExtSym(get_color);
    scAddExtSym(get_pallete);
    scAddExtSym(get_pallete_range);
    scAddExtSym(generate_332_palette);
    scAddExtSym(generate_optimized_palette);
    scAddExtSym(bestfit_color);
    scAddExtSym(makecol);
    scAddExtSym(makecol8);
    scAddExtSym(makecol_depth);
    scAddExtSym(getr);
    scAddExtSym(getg);
    scAddExtSym(getb);
    scAddExtSym(getr_depth);
    scAddExtSym(getg_depth);
    scAddExtSym(getb_depth);
    scAddExtSym(putpixel);
    scAddExtSym(set_clip);
    scAddExtSym(drawing_mode);
    scAddExtSym(xor_mode);
    scAddExtSym(solid_mode);
    scAddExtSym(do_line);
    scAddExtSym(triangle);
    scAddExtSym(polygon);
    scAddExtSym(rect);
    scAddExtSym(do_circle);
    scAddExtSym(circle);
    scAddExtSym(circlefill);
    scAddExtSym(do_ellipse);
    scAddExtSym(ellipse);
    scAddExtSym(ellipsefill);
    scAddExtSym(calc_spline);
    scAddExtSym(spline);
    scAddExtSym(floodfill);
    scAddExtSym(blit);
    scAddExtSym(masked_blit);
    scAddExtSym(stretch_blit);
    scAddExtSym(stretch_sprite);
    scAddExtSym(rotate_sprite);
    scAddExtSym(rotate_scaled_sprite);
    //scAddExtSym(draw_gouraud_sprite);
    scAddExtSym(draw_trans_sprite);
    scAddExtSym(clear);
    
    scAddExtSym(text_mode);
    scAddExtSym(textout);
    scAddExtSym(textout_centre);
    scAddExtSym(textout_justify);
    scAddExtSym(textprintf);
    scAddExtSym(textprintf_centre);
    scAddExtSym(text_length);
    scAddExtSym(text_height);
    scAddExtSym(destroy_font);
    
    scAddExtSym(play_fli);
    scAddExtSym(play_memory_fli);
    scAddExtSym(open_fli);
    scAddExtSym(open_memory_fli);
    scAddExtSym(close_fli);
    scAddExtSym(next_fli_frame);
    scAddExtSym(reset_fli_variables);
    scAddExtSym(fli_bitmap);             /* current frame of the FLI */
    scAddExtSym(fli_pallete);            /* current FLI pallete */
    scAddExtSym(fli_frame);                  /* current frame number */

    scAddExtSym(load_datafile);
    scAddExtSym(unload_datafile);

    scAddExtSym(install_sound);
    scAddExtSym(set_volume);
    scAddExtSym(load_sample);
    scAddExtSym(load_wav);
    scAddExtSym(load_voc);
    //scAddExtSym(create_sample);
    scAddExtSym(destroy_sample);
    scAddExtSym(play_sample);
    scAddExtSym(stop_sample);
    scAddExtSym(adjust_sample);
    
    scAddExtSym(load_midi);
    scAddExtSym(destroy_midi);
    scAddExtSym(play_midi);
    scAddExtSym(play_looped_midi);
    scAddExtSym(stop_midi);
    scAddExtSym(midi_pause);
    scAddExtSym(midi_resume);
    scAddExtSym(midi_seek);
    scAddExtSym(midi_out);
    scAddExtSym(load_midi_patches);
    
    //FILES:
    scAddExtSym(packfile_password);
    scAddExtSym(pack_fopen);
    scAddExtSym(pack_fclose);
    scAddExtSym(pack_fseek);
    scAddExtSym(pack_fopen_chunk);
    scAddExtSym(pack_fclose_chunk);
    scAddExtSym(pack_igetw);
    scAddExtSym(pack_igetl);
    scAddExtSym(pack_iputw);
    scAddExtSym(pack_iputl);
    scAddExtSym(pack_mgetw);
    scAddExtSym(pack_mgetl);
    scAddExtSym(pack_mputw);
    scAddExtSym(pack_mputl);
    scAddExtSym(pack_fread);
    scAddExtSym(pack_fwrite);
    scAddExtSym(pack_fgets);
    scAddExtSym(pack_fputs);
    
    scAddExtSym(get_filename);
    scAddExtSym(load_bitmap);
    scAddExtSym(load_bmp);
    scAddExtSym(load_lbm);
    scAddExtSym(load_pcx);
    scAddExtSym(load_tga);
    
    scAddExtSym(save_bitmap);
    scAddExtSym(save_bmp);
    scAddExtSym(save_pcx);
    scAddExtSym(save_tga);
    
    scAddExtSym(create_rgb_table);
    scAddExtSym(create_light_table);
    scAddExtSym(create_trans_table);
    scAddExtSym(create_color_table);
    scAddExtSym(rgb_map);
    scAddExtSym(color_map);
    //scAddExtSym(_mouse_installed);

    /* gcc complains about stripping the "volatile" 
     * part off, so we shut it up
     */
    scAdd_External_Symbol("mouse_x", (int *)&mouse_x);
    scAdd_External_Symbol("mouse_y", (int *)&mouse_y);
    scAdd_External_Symbol("mouse_b", (int *)&mouse_b);
    scAdd_External_Symbol("mouse_pos", (int *)&mouse_pos);
    
    scAddExtSym(freeze_mouse_flag);
    scAddExtSym(keypressed);
    scAddExtSym(readkey);
}

