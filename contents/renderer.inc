<?php
/**
 * Hardcoded document renderer
 *
 * @package    BardCanvas
 * @subpackage Hardcoded Pages
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * @var template $template
 * @var document $document
 * @var account  $author
 */

use hng2_base\account;
use hng2_base\template;
use hng2_modules\hcpages\document;

$tag = $document->layout == "main" ? "article" : "div";
?>
<<?=$tag?> class="post single_post hcpage clearfix" data-docpath="<?= $document->path ?>"
         data-author-id="<?= $author->id_account ?>" data-author-role="<?= $author->get_role(true) ?>"
         data-author-level="<?= $author->level ?>" style="<? if($document->hidden) echo "background-color: #ffeeb6"; ?>">
    
    <? if( $document->layout != "embeddable" ): ?>
        <h1>
            <?= $document->get_processed_title() ?>
        </h1>
        
        <? if( $document->layout != "popup" ): ?>
            <p class="single_item_actions">
                <span class="item action clipboard-copy" data-clipboard-text="<?= $document->get_permalink(true) ?>" style="">
                    <?= $language->permalink ?>
                    <span class="fa fa-clipboard"></span>
                </span>
                <? if( $account->has_admin_rights_to_module("hcpages") ): ?>
                    <a class="item action edit"
                       href="<?= $current_module->get_url() ?>?edit=<?= urlencode($document->path) ?>&return_to_self=true">
                        <i class="fa fa-pencil fa-fw"></i>
                        <?= $current_module->language->actions->edit ?>
                    </a>
                    <? if( $document->hidden ): ?>
                        <span class="item action show" onclick="toggle_file('visible', '<?= addslashes($document->path) ?>')">
                            <i class="fa fa-eye"></i>
                            <?= $current_module->language->actions->show ?>
                        </span>
                    <? else: ?>
                        <span class="item action hide" onclick="toggle_file('hidden', '<?= addslashes($document->path) ?>')">
                            <i class="fa fa-eye-slash"></i>
                            <?= $current_module->language->actions->hide ?>
                        </span>
                    <? endif; ?>
                <? endif; ?>
            </p>
        <? endif; ?>
    <? endif; ?>
    
    <div class="post_contents">
        <?= $document->get_processed_content() ?>
    </div>
    
</<?=$tag?>>

<? if( $document->layout != "embeddable" ): ?>
    <script type="text/javascript">
        function toggle_file(visibility, path)
        {
            var url      = $_FULL_ROOT_PATH + '/hcpages/';
            var params   = {
                toggle:     path,
                visibility: visibility,
                wasuuup:    wasuuup()
            }
            
            $.blockUI(blockUI_default_params);
            $.get(url, params, function(response)
            {
                if( response !== 'OK' )
                {
                    $.unblockUI();
                    throw_notification(response, 'warning');
                    
                    return;
                }
    
                $.unblockUI();
                location.href = $_FULL_ROOT_PATH + '/' + path;
            });
        }
    </script>
<? endif; ?>
