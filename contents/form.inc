<?php
/**
 * Hardcoded Pages form
 *
 * @package    BardCanvas
 * @subpackage Hardcoded Pages
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * Trailing vars:
 * @var document $document
 */

use hng2_modules\hcpages\document;

$is_dupe   = ! empty($_GET["duplicate"]);
$return_to = $_GET["return_to_self"] == "true"
           ? "{$config->full_root_path}/{$document->path}"
           : ("?wasuuup=" . md5(mt_rand(1, 65535)));

?>

<h1 class="clearfix">
    
    <?= $template->get_page_title(false) ?>
    
    <a id="return_link" class="framed_content inlined pseudo_link pull-right" href="<?= $return_to ?>">
        <i class="fa fa-reply"></i>
        <?= $language->return ?>
    </a>
    
</h1>

<form id="composer" method="post" action="?mode=save">
    
    <input type="hidden" name="original_path" value="<?= $is_dupe ? "" : $document->path ?>">
    <input type="hidden" name="hidden" value="<?= $document->hidden ?>">
    <input type="hidden" name="preview" value="false">
    <input type="hidden" name="keep_open" value="false">
    
    <div class="multicol cols-2">
        <div class="col">
            <!-- Title -->
            <div class="field">
                <div class="caption">
                    <?= $current_module->language->form->title->caption ?>
                </div>
                <div class="input">
                    <textarea name="title" class="expandible_textarea no_fw_font"
                              placeholder="<?= $current_module->language->form->title->placeholder ?>"
                              onkeyup="forge_document_path()" onblur="forge_document_path()"><?
                        echo htmlspecialchars($document->title);
                        ?></textarea>
                </div>
            </div>
            
            <!-- Path -->
            <div class="field">
                <div class="caption">
                    <?= $current_module->language->form->path->caption ?>
                </div>
                <div class="input">
                    <input type="text" name="path" <? if( ! empty($document->path) && ! $is_dupe ) echo "readonly"; ?>
                           value="<?= htmlspecialchars($document->path) ?>"
                           placeholder="<?= $current_module->language->form->path->placeholder ?>">
                </div>
            </div>
            
            <!-- Layout -->
            <div class="field">
                <div class="caption">
                    <?= $current_module->language->form->layout->caption ?>
                </div>
                <div class="input">
                    <? foreach($current_module->language->form->layout->options->children() as $node):
                        $val = $node->getName();
                        $sel = $document->layout == $val ? "checked" : "";
                        ?>
                        <label>
                            <input type="radio" name="layout" <?= $sel ?> value="<?= $val ?>">
                            <?= $node->caption ?>
                        </label>
                        <em><?= $node->info ?></em><br>
                    <? endforeach; ?>
                </div>
            </div>
            
            <!-- Page tag -->
            <div class="field">
                <div class="caption">
                    <?= $current_module->language->form->tag->caption ?>
                </div>
                <div class="input">
                    <input type="text" name="tag" value="<?= htmlspecialchars($document->tag) ?>"
                           placeholder="<?= $current_module->language->form->tag->placeholder ?>">
                </div>
            </div>
        </div>
        <div class="col">
            
            <!-- Description -->
            <div class="field">
                <div class="caption">
                    <?= $current_module->language->form->description->caption ?>
                </div>
                <div class="input">
                    <textarea name="description" class="expandible_textarea"
                              placeholder="<?= $current_module->language->form->description->placeholder ?>"
                              style="min-height: 3.4em;"><?
                        echo htmlspecialchars($document->description);
                        ?></textarea>
                </div>
            </div>
            
            <!-- Featured image -->
            <div class="field">
                <div class="caption">
                    <?= $current_module->language->form->featured_image->caption ?>
                </div>
                <div class="input featured_image aligncenter">
                    <input type="hidden" name="featured_image_path" value="<?= $document->featured_image_path ?>">
                    <div class="thumbnail preview featured_image pseudo_link"
                         onclick="set_doc_featured_image()">
                        <? $image_path = empty($document->featured_image_path)
                            ? "{$config->full_root_path}/media/missing_image.png"
                            : $document->featured_image_path; ?>
                        <img src="<?= $image_path ?>"
                             data-empty-src="<?= $config->full_root_path ?>/media/missing_image.png">
                    </div>
                    <div align="center">
                        <span class="pseudo_link" onclick="set_doc_featured_image()">
                            <?= $current_module->language->form->featured_image->set ?>
                        </span>
                        &nbsp;
                        <span class="pseudo_link" onclick="remove_doc_featured_image()">
                            <?= $current_module->language->form->featured_image->remove ?>
                        </span>
                    </div>
                </div>
            </div>
        </div>
    </div>
    
    <!-- Content -->
    <div class="field tinymce_container">
        <textarea name="content" id="post_content_editor" class="tinymce_full"
                  placeholder="<?= $current_module->language->form->content->placeholder ?>"><?
            echo htmlspecialchars($document->content);
            ?></textarea>
    </div>
    
    <!-- Markup fields -->
    
    <fieldset>
        <legend><?= $current_module->language->form->html_head ?></legend>
        <textarea class="CodeMirror" mode="htmlmixed" id="head_markup_ta" name="head_markup"><?
            echo htmlspecialchars($document->head_markup);
            ?></textarea>
    </fieldset>
    
    <br>
    
    <fieldset>
        <legend><?= $current_module->language->form->pre_content ?></legend>
        <textarea class="CodeMirror" mode="htmlmixed" id="pre_content_markup_ta" name="pre_content_markup"><?
            echo htmlspecialchars($document->pre_content_markup);
            ?></textarea>
    </fieldset>
    
    <br>
    
    <fieldset>
        <legend><?= $current_module->language->form->post_content ?></legend>
        <textarea class="CodeMirror" mode="htmlmixed" id="post_content_markup_ta" name="post_content_markup"><?
            echo htmlspecialchars($document->post_content_markup);
            ?></textarea>
    </fieldset>
    
    <!-- Custom fields -->
    
    <br>
    <fieldset id="custom_fields_container">
        <legend><?= $current_module->language->form->custom_fields->legend ?></legend>
        <div id="custom_fields" class="multicol cols-2">
            <? if( ! empty($document->custom_fields) ):
                foreach($document->custom_fields as $key => $val):
                    ?>
                    <div class="custom_field col field">
                        <div class="caption nowrap">
                            <input type="text" name="custom_field_names[]" value="<?= $key ?>"
                                   placeholder="<?= $current_module->language->form->custom_fields->name ?>"
                                   style="width: calc(100% - 24px)">
                            <i class="fa fa-trash fa-fw fa-lg pseudo_link"
                               onclick="remove_custom_field(this)"></i>
                        </div>
                        <div class="input">
                            <textarea name="custom_field_vals[]" rows="3" wrap="off"
                                      placeholder="<?= $current_module->language->form->custom_fields->value ?>"><?
                                echo htmlspecialchars($val);
                            ?></textarea>
                        </div>
                    </div>
                    <?
                endforeach;
            endif; ?>
        </div>
        <div class="add_field">
            <span class="framed_content inlined state_ok pseudo_link" onclick="add_custom_field()">
                <i class="fa fa-plus"></i>
                <?= $current_module->language->form->custom_fields->add ?>
            </span>
        </div>
    </fieldset>
    
    <!-- Buttons -->
    
    <p align="center">
        <button type="reset" class="big_form_submit_button"
                onclick="location.href = '<?= $return_to ?>'">
            <i class="fa fa-ban"></i>
            <?= $current_module->language->form->cancel ?>
        </button>
        <!--
        &nbsp;
        <button type="submit" class="big_form_submit_button" onclick="$('input[name=preview]').val('true')">
            <i class="fa fa-search"></i>
            <?= $current_module->language->form->preview ?>
        </button>
        -->
        &nbsp;
        <button type="submit" class="big_form_submit_button" onclick="$('input[name=keep_open]').val('false')">
            <i class="fa fa-check"></i>
            <?= $current_module->language->form->save_and_close ?>
        </button>
        &nbsp;
        <button type="submit" class="big_form_submit_button" onclick="$('input[name=keep_open]').val('true')">
            <i class="fa fa-save"></i>
            <?= $current_module->language->form->just_save ?>
        </button>
    </p>
    
</form>

<div id="composer_target" style="display: none"></div>

<template id="custom_field_template">
    <div class="custom_field col field">
        <div class="caption nowrap">
            <input type="text" name="custom_field_names[]"
                   placeholder="<?= $current_module->language->form->custom_fields->name ?>"
                   style="width: calc(100% - 24px)">
            <i class="fa fa-trash fa-fw fa-lg pseudo_link"
               onclick="remove_custom_field(this)"></i>
        </div>
        <div class="input">
            <textarea name="custom_field_vals[]" rows="3" wrap="off"
                      placeholder="<?= $current_module->language->form->custom_fields->value ?>"></textarea>
        </div>
    </div>
</template>

<style type="text/css">
    #composer .featured_image .thumbnail {
        border: 1px solid; background-color: transparent;
        position: relative; width: 100%; height: 190px; overflow: hidden;
    }

    #composer .featured_image .thumbnail img {
        position: absolute; left: 50%; top: 50%; max-width: 100%; height: auto;
        -webkit-transform: translate(-50%,-50%);
        -ms-transform:     translate(-50%,-50%);
        transform:         translate(-50%,-50%);
    }
    
    /*
    .custom_field                       { display: flex; flex-wrap: wrap; }
    .custom_field .input_name           { flex-grow: 4; padding: 4px; }
    .custom_field .input_name     input { width:100%; }
    .custom_field .input_val            { flex-grow: 6; padding: 4px; }
    .custom_field .field_buttons        { white-space: nowrap; padding: 4px; }
    .custom_field .field_buttons  .fa   { position: relative; top: -2px; }
    */
    
</style>

<script type="text/javascript">
    
    var $composer = $('#composer');
    var slug_sep  = '<?= $settings->get("engine.slugs_word_separator") ?>' === 'dashes' ? '-' : '_';
    
    function forge_document_path()
    {
        var $title = $composer.find('textarea[name="title"]');
        var $path  = $composer.find('input[name="path"]');
        if( $path.prop('readonly') === true ) return;
        // if( $path.val() !== '' ) return;
        
        var final_path = $title.val()
                               .trim()
                               .replace(/\n+/g,               ' ')
                               .replace(/[áàäâ]/gi,           'a')
                               .replace(/[éèëê]/gi,           'e')
                               .replace(/[íìëî]/gi,           'i')
                               .replace(/[óòöô]/gi,           'o')
                               .replace(/[úùüû]/gi,           'u')
                               .replace(/[ñ]/gi,              'n')
                               .replace(/[ç]/gi,              'c')
                               .replace(/[^a-z0-9_.\-\/ ]/gi, '')
                               .replace(/ +/g,                slug_sep)
                               .replace(/_+/g,                '_')
                               .replace(/-+/g,                '-')
                               .replace(/\.+/g,               '.')
                               .replace(/\.$/g,                '')
                               .toLowerCase();
        
        if( final_path.length > 200 ) final_path = final_path.substring(0, 200);
        
        $path.val( final_path );
    }
    
    function set_doc_featured_image()
    {
        var editor_id = $composer.find('textarea[class*="tinymce"]').attr('id');
        var editor    = tinymce.get(editor_id);
        
        load_media_browser_in_tinymce_dialog(
            editor,
            $(window).width() - 20,
            $(window).height() - 60,
            'image',
            'parent.' + 'set_selected_gallery_image_as_featured_image'
        );
    }
    
    function set_selected_gallery_image_as_featured_image(
        id_media, type, file_url, thumbnail_url // , width, height, embed_width
    ) {
        var $strings = $('#post_gallery_embed_strings');
        
        if( type !== 'image' )
        {
            var message = $strings.find('.invalid_type_for_image').text();
            throw_notification( message, 'warning' );
            
            return;
        }
        
        tinymce.activeEditor.windowManager.close();
        
        var $form = $composer;
        $form.find('input[name="featured_image_path"]').val(file_url);
        $form.find('.input.featured_image .thumbnail img').attr('src', thumbnail_url);
    }
    
    function remove_doc_featured_image()
    {
        var $form     = $composer;
        var empty_src = $form.find('.input.featured_image .thumbnail img').attr('data-empty-src');
        $form.find('input[name="featured_image_path"]').val('');
        $form.find('.input.featured_image .thumbnail img').attr('src', empty_src);
    }
    
    function add_custom_field()
    {
        var $item = $( $('#custom_field_template').html() );
        $('#custom_fields').append($item);
        $item.find('input[name="custom_field_names[]"]').focus();
    }
    
    function remove_custom_field(trigger)
    {
        $(trigger).closest('.custom_field').fadeOut('fast', function() { $(this).remove(); });
    }
    
    function prepare_form_serialization($form)
    {
        $form.find('textarea[class*="tinymce"]').each(function()
        {
            var id      = $(this).attr('id');
            var editor  = tinymce.get(id);
            var content = editor.getContent();
            $(this).val( content );
        });
        
        $form.find('textarea[class="CodeMirror"]').each(function()
        {
            var $this  = $(this);
            var editor = $this.data('editor');
            var markup = editor.getValue();
            $this.val( markup );
        });
    }
    
    function prepare_form_submission()
    {
        $.blockUI(blockUI_default_params);
    }
    
    function process_form_response(response, statusText, xhr, $form)
    {
        if( response !== 'OK' )
        {
            $.unblockUI();
            throw_notification(response, 'warning');
            
            return;
        }
        
        if( $form.find('input[name=keep_open]').val() === 'true' )
            $.unblockUI();
        else
            location.href = '?';
    }
    
    $composer.ajaxForm({
        target:          '#composer_target',
        beforeSerialize: prepare_form_serialization,
        beforeSubmit:    prepare_form_submission,
        success:         process_form_response
    });
    
</script>

<!-- Expandible textarea -->
<script type="text/javascript" src="<?= $config->full_root_path ?>/lib/jquery.exptextarea.js"></script>
<script type="text/javascript">$(document).ready(function() { $('.expandible_textarea').expandingTextArea() });</script>

<!-- CodeMirror -->
<link rel="stylesheet" href="<?=$config->full_root_path?>/lib/CodeMirror-5.16.2/lib/codemirror.css">
<link rel="stylesheet" href="<?=$config->full_root_path?>/lib/CodeMirror-5.16.2/addon/fold/foldgutter.css">
<script                 src="<?=$config->full_root_path?>/lib/CodeMirror-5.16.2/lib/codemirror.js"></script>
<script                 src="<?=$config->full_root_path?>/lib/CodeMirror-5.16.2/addon/fold/foldcode.js"></script>
<script                 src="<?=$config->full_root_path?>/lib/CodeMirror-5.16.2/addon/fold/foldgutter.js"></script>
<script                 src="<?=$config->full_root_path?>/lib/CodeMirror-5.16.2/addon/fold/brace-fold.js"></script>
<script                 src="<?=$config->full_root_path?>/lib/CodeMirror-5.16.2/addon/fold/xml-fold.js"></script>
<script                 src="<?=$config->full_root_path?>/lib/CodeMirror-5.16.2/addon/fold/indent-fold.js"></script>
<script                 src="<?=$config->full_root_path?>/lib/CodeMirror-5.16.2/addon/fold/markdown-fold.js"></script>
<script                 src="<?=$config->full_root_path?>/lib/CodeMirror-5.16.2/addon/fold/comment-fold.js"></script>
<script                 src="<?=$config->full_root_path?>/lib/CodeMirror-5.16.2/mode/xml/xml.js"></script>
<script                 src="<?=$config->full_root_path?>/lib/CodeMirror-5.16.2/mode/javascript/javascript.js"></script>
<script                 src="<?=$config->full_root_path?>/lib/CodeMirror-5.16.2/mode/css/css.js"></script>
<script                 src="<?=$config->full_root_path?>/lib/CodeMirror-5.16.2/mode/htmlmixed/htmlmixed.js"></script>
<style type="text/css">
    .CodeMirror {
        border:      1px solid silver;
        height:      auto;
        font-family: 'Lucida Console', 'Courier new', Courier, monospace !important; font-size: 8pt;
        width:       100%;
    }
    .CodeMirror-scroll { min-height: 100px; }
</style>
<script type="text/javascript">
    $(document).ready(function()
    {
        $('textarea.CodeMirror').each(function()
        {
            var $this   = $(this);
            var mode    = $this.attr('mode');
            var element = $this.get(0);
            
            var ed = CodeMirror.fromTextArea(element, {
                viewPortMargin: Infinity,
                lineNumbers:    true,
                mode:           mode,
                foldGutter:     true,
                gutters:        ["CodeMirror-linenumbers", "CodeMirror-foldgutter"]
            });
            $this.data('editor', ed);
        });
    });
</script>
