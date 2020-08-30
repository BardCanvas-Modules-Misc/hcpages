<?php
/**
 * Hardcoded Pages index
 *
 * @package    BardCanvas
 * @subpackage Hardcoded Pages
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * Trailing vars:
 * @var string $error
 */

use hng2_base\account;
use hng2_base\accounts_repository;
use hng2_modules\hcpages\document;

$path = "{$config->datafiles_location}/hcpages";
if( ! is_dir($path) )
    if( ! @mkdir($path) )
        $error = $current_module->language->messages->cannot_create_path;

/** @var document[] $all_pages */
$all_pages = array();

/** @var account[] $authors */
$all_authors = array();

$author_ids = array();
$all_paths  = array();
if( is_dir($path) )
{
    $all_entries = get_dir_contents($path);
    foreach($all_entries as $file)
    {
        if( is_file($file) )
        {
            $document    = new document($file);
            $all_paths[] = dirname("/{$document->path}");
            if( ! empty($document->created_by) ) $author_ids[] = $document->created_by;
            $all_pages[] = $document;
        }
    }
    
    $all_paths = array_unique($all_paths);
    if( count($all_paths) == 1 ) $all_paths = array();
}

switch($account->engine_prefs["@hcpages:browser.display_order"])
{
    case "newest":   usort($all_pages, function($a, $b) { return $a->creation_date < $b->creation_date; }); break;
    case "oldest":   usort($all_pages, function($a, $b) { return $a->creation_date > $b->creation_date; }); break;
    case "updated":  usort($all_pages, function($a, $b) { return $a->last_update   < $b->last_update;   }); break;
    case "-updated": usort($all_pages, function($a, $b) { return $a->last_update   > $b->last_update;   }); break;
    case "title":    usort($all_pages, function($a, $b) { return $a->title         > $b->title;         }); break;
    case "page_tag":
        usort($all_pages, function($a, $b) { return "$a->tag | $a->path" > "$b->tag | $b->path"; }); break;
    default: # directory + title
        usort($all_pages, function($a, $b) {
            $apath = dirname("/$a->path"); $bpath = dirname("/$b->path");
            return "$apath | $a->title" > "$bpath | $b->title";
        }); break;
}

if( ! empty($author_ids) )
{
    $accounts_repo = new accounts_repository();
    $all_authors   = $accounts_repo->get_multiple($author_ids);
}

$preselected_path = "";
?>

<h1 class="clearfix">
    
    <?= $current_module->language->index->title ?>
    
    <? if( ! empty($all_paths) ): ?>
        <select id="path_filter" onchange="filter_files(this.options[this.selectedIndex].value)">
            <option selected value=""><?= $current_module->language->all_paths ?></option>
            <? foreach($all_paths as $path):
                $selected = $account->engine_prefs["@hcpages:files_filter"] == $path ? "selected" : "";
                if( $selected ) $preselected_path = $path; ?>
                <option <?= $selected ?> value="<?= $path ?>"><?= $path ?></option>
            <? endforeach; ?>
        </select>
    <? endif; ?>
    
    <? if( ! empty($all_pages) ): ?>
        <select onchange="var val = this.options[this.selectedIndex].value; $(this).block(blockUI_smallest_params); set_engine_pref('@hcpages:browser.display_order', val, function() { $('#docs_browser_refresher').click(); })">
            <? foreach($current_module->language->index->sorting->option as $option)
                echo sprintf(
                    "<option %s value='%s'>%s</option>",
                    $account->engine_prefs["@hcpages:browser.display_order"] == trim($option["value"]) ? "selected" : "",
                    $option["value"],
                    $option
                ); ?>
        </select>
    <? endif; ?>
    
    <button onclick="location.href = '?mode=create'">
        <i class="fa fa-plus"></i>
        <?= $current_module->language->index->create ?>
    </button>
    
    <button id="docs_browser_refresher" class="pull-right"
            onclick="location.href = '?wasuuup=<?= md5(mt_rand(1, 65535)) ?>'">
        <i class="fa fa-refresh"></i>
        <?= $language->reload ?>
    </button>
    
</h1>

<?
if( ! empty($error) )
{
    echo "
        <div class='framed_content state_ko'>
            <i class='fa fa-warning'></i>
            {$error}
        </div>
    ";
}

if( empty($all_pages) )
{
    echo "
        <div class='framed_content state_ko' style='padding: 20px'>
            <i class='fa fa-info-circle'></i>
            {$current_module->language->messages->no_pages_found}
        </div>
    ";
    
    return;
}

$class = empty($all_paths) ? "no_paths" : "";
?>

<div class="pages_browser <?= $class ?>">
    <? foreach($all_pages as $document):
        $class  = $document->hidden ? "page_hidden" : "";
        $path   = dirname("/{$document->path}");
        $style  = empty($preselected_path) ? "" : ($path == $preselected_path ? "" : "display: none");
        ?>
        <div class="page_item content_frame <?= $class ?>" data-path="<?= $path ?>" data-docpath="<?= $document->path ?>" 
             style="<?= $style ?>">
            <div class="path"><?= $path ?></div>
            <? if( empty($document->featured_image_path) ): ?>
                <div class="featured_image" style="background-color: silver; background-image: url('<?= $config->full_root_path ?>/media/missing_image.png'); background-size: 64px;"></div>
            <? else: ?>
                <div class="featured_image" style="background-image: url('<?= $document->featured_image_path ?>');"></div>
            <? endif; ?>
            <div class="title principal_data"><?= $document->title ?></div>
            <div class="description"><?= $document->description ?></div>
            <div class="meta" style="height: 1rem;">
                <? if( ! empty($document->tag) ): ?>
                    <span><span class='alternate inline-block nowrap'
                          style='width: 100%; overflow: hidden; text-overflow: ellipsis'><i 
                          class='fa fa-hashtag fa-fw'></i><?= $document->tag ?></span></span>
                <? endif; ?>
            </div>
            
            <div class="meta">
                <?
                switch($document->layout)
                {
                    case "main":       $icon = "fa-file-text-o"; break;
                    case "admin":      $icon = "fa-file";        break;
                    case "popup":      $icon = "fa-file-o";      break;
                    case "embeddable": $icon = "fa-cube";        break;
                }
                ?>
                <span class="page_layout important"><i
                      class="fa <?= $icon ?> fa-fw"></i><?
                    echo $current_module->language->form->layout->options->{$document->layout}->caption
                    ?></span>
                
                <? if( ! empty($document->creation_date) ): ?>
                    <span class="page_creation_date important"><i
                          class="fa fa-plus-square-o fa-fw"></i><?= time_today_string($document->creation_date) ?></span>
                <? endif; ?>
                
                <span class="page_last_update important"><i
                      class="fa fa-clock-o fa-fw"></i><?= time_elapsed_string($document->last_update) ?></span>
                
                <? if( $account->id_account != $document->created_by && $all_authors[$document->created_by] ):
                    $author = $all_authors[$document->created_by]; ?>
                    <a class="important page_created_by user_display_name" data-user-level="<?= $author->id_account ?>"
                       href="<?= $config->full_root_path ?>/user/<?= $author->user_name ?>"><i
                       class="fa fa fa-user fa-fw"></i><?= $author->get_processed_display_name() ?></a>
                <? endif; ?>
            </div>
            <div class="actions">
                <a class="action open" href="<?= $document->get_permalink() ?>" target="_blank"><i
                   class="fa fa-link fa-fw"></i><?= $current_module->language->actions->open ?></a>
                <a class="action edit greengo" href="<?= $_SERVER["PHP_SELF"] ?>?edit=<?= urlencode($document->path) ?>"><i
                   class="fa fa-pencil fa-fw"></i><?= $current_module->language->actions->edit ?></a>
                <a class="action duplicate alternate" href="<?= $_SERVER["PHP_SELF"] ?>?duplicate=<?= urlencode($document->path) ?>"><i
                   class="fa fa-copy fa-fw"></i><?= $current_module->language->actions->duplicate ?></a>
                <span class="action show important pseudo_link" onclick="toggle_file(this, 'visible')"><i
                      class="fa fa-eye fa-fw"></i><?= $current_module->language->actions->show ?></span>
                <span class="action hide important pseudo_link" onclick="toggle_file(this, 'hidden')"><i
                      class="fa fa-eye-slash fa-fw"></i><?= $current_module->language->actions->hide ?></span>
                <span class="action delete critical pseudo_link" onclick="delete_file(this)"><i
                      class="fa fa-times fa-fw"></i><?= $current_module->language->actions->delete ?></span>
            </div>
        </div>
    <? endforeach; ?>
</div>

<style type="text/css">
    .page_item { display: inline-block; width: 200px; margin: 10px; padding: 0; vertical-align: top;
                 font-family: Arial, Helvetica, sans-serif; font-size: 10pt; line-height: normal; }
    
    @media all and (min-width:  100px) { .page_item { width: calc(100% - 20px) } }
    @media all and (min-width:  300px) { .page_item { width: calc( 50% - 23px) } }
    @media all and (min-width:  700px) { .page_item { width: calc( 33% - 23px) } }
    @media all and (min-width: 1024px) { .page_item { width: calc( 20% - 25px) } }
    @media all and (min-width: 1360px) { .page_item { width: calc( 15% -  3px) } }
    @media all and (min-width: 1920px) { .page_item { width: calc( 12% - 15px) } }
    
    .page_item .path            { font-family: "Lucida Console", "Courier New", monospace; text-align: center;
                                  font-size: 7pt; line-height: normal; padding: 2px;
                                  color: white; background-color: dimgrey;
                                  white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }
    .page_item .featured_image  { background: center center no-repeat; background-size: cover; height: 100px; }
    .page_item .title           { margin-top: 10px; padding: 0 5px; height: 2.3em; overflow: hidden; }
    .page_item .description     { margin-top: 10px; padding: 0 5px; height: 3.4em; overflow: hidden; }
    .page_item .meta            { margin-top: 10px; padding: 0 5px; font-size: 9pt; height: 3em; overflow: hidden; }
    .page_item .actions         { margin: 10px 5px 5px 5px; text-align: center;}
    .page_item .actions .action { display: inline-block; margin: 0 5px 5px 0; white-space: nowrap; }

    .pages_browser .meta > * { display: inline-block; white-space: nowrap; color: darkmagenta; margin: 0 5px 5px 0; }
    
    .pages_browser.no_paths .page_item .path { display: none; }
    
    .page_item.page_hidden                 { background-color: silver; color: dimgrey; }
    .page_item.page_hidden .featured_image { filter: grayscale(100%); opacity: 0.75; }
    
    .page_item             .actions .action.show { display: none; }
    .page_item.page_hidden .actions .action.show { display: inline-block; }
    .page_item.page_hidden .actions .action.hide { display: none; }

</style>

<script type="text/javascript">
    
    function filter_files(path)
    {
        var $items = $('.page_item');
        
        set_engine_pref('@hcpages:files_filter', path);
        
        if( path === '' )
        {
            $items.show();
            
            return;
        }
        
        $items.each(function()
        {
            var $this    = $(this);
            var thispath = $this.attr('data-path');
            $this.toggle(thispath === path);
        });
    }
    
    function toggle_file(trigger, visibility)
    {
        var $trigger = $(trigger);
        var $item    = $trigger.closest('.page_item');
        var path     = $item.attr('data-docpath');
        var url      = $_PHP_SELF;
        var params   = {
            toggle:     path,
            visibility: visibility,
            wasuuup:    wasuuup()
        }
        
        $item.block(blockUI_smaller_params);
        $.get(url, params, function(response)
        {
            if( response !== 'OK' )
            {
                $item.unblock();
                throw_notification(response, 'warning');
                
                return;
            }
            
            $item.unblock();
            if( visibility === 'hidden' ) $item.toggleClass('page_hidden', true);
            else                          $item.toggleClass('page_hidden', false);
        });
    }
    
    function delete_file(trigger)
    {
        if( ! confirm($_GENERIC_CONFIRMATION) ) return;
        
        var $trigger = $(trigger);
        var $item    = $trigger.closest('.page_item');
        var path     = $item.attr('data-docpath');
        var url      = $_PHP_SELF;
        var params   = {
            delete:  path,
            wasuuup: wasuuup()
        }
        
        $item.block(blockUI_smaller_params);
        $.get(url, params, function(response)
        {
            if( response !== 'OK' )
            {
                $item.unblock();
                throw_notification(response, 'warning');
                
                return;
            }
            
            $item.unblock();
            $item.fadeOut('fast', function() { $(this).remove(); });
        });
    }
    
</script>

<?
/**
 * @param string $dir
 * @param array  $results
 *
 * @return array
 */
function get_dir_contents($dir, &$results = array())
{
    $files = glob("$dir/*.xml");
    $files = array_merge($files, glob("$dir/*", GLOB_ONLYDIR));
    
    foreach( $files as $path )
    {
        if( is_dir($path) )
        {
            $node = basename($path);
            if( $node != "." && $node != ".." )
            {
                get_dir_contents($path, $results);
                $results[] = $path;
            }
        }
        else
        {
            $results[] = $path;
        }
    }
    
    return $results;
}
