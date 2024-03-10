<?php
/**
 * Document handler for hardcoded documents
 * 
 * @package    BardCanvas
 * @subpackage Hardcoded Pages
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * @var template $template
 * 
 * $_GET params:
 * @param "handle"
 */

use hng2_base\account;
use hng2_base\template;
use hng2_media\media_repository;
use hng2_modules\hcpages\document;

if( empty($_GET["handle"]) ) return;

$handle = trim(stripslashes($_GET["handle"]));
$file = "{$config->datafiles_location}/hcpages/{$handle}.xml";

if( ! is_file($file) ) return;

$document = new document($file);
if( $document->hidden && ! $account->has_admin_rights_to_module("hcpages") ) throw_fake_401();

$author = new account();
if( ! empty($document->created_by) ) $author = new account($document->created_by);

$config->globals["@hcpages:called_from_page_renderer"] = true;
$template->set_page_title($document->title);
$template->page_meta_description       = $document->description;
$template->page_meta_url               = $document->get_permalink(true);
$template->page_extra_metas["og:type"] = "<meta property='og:type' content='article'>";
if( ! empty($document->featured_image_path) )
{
    $mrepo = new media_repository();
    $filter = array("path" => str_replace("/mediaserver/", "", $document->featured_image_path), "visibility" => "public");
    $items  = $mrepo->find($filter, 1, 0, "creation_date desc");
    if( count($items) )
    {
        $template->page_meta_image = $config->full_root_url . "/mediaserver/" . $items[0]->thumbnail;
    }
    else
    {
        $default_image = $settings->get("engine.meta_default_image");
        if( ! empty($default_image) ) $template->page_meta_image = "{$config->full_root_url}{$default_image}";
    }
}

if( preg_match("#(<img.*data-media-type=.video..*>)#i", $document->content, $matches) )
{
    $tag = $matches[1];
    if( preg_match("#data-id-media=.([0-9]+).#i", $tag, $matches) )
    {
        $mid   = $matches[1];
        $mrepo = new media_repository();
        $mitem = $mrepo->get($mid);
        if( is_object($mitem) && $mitem->visibility == "public" && $mitem->status == "published")
        {
            $template->page_extra_metas["og:video"]        = "<meta property='og:video'        content='{$mitem->get_item_url(true)}'>";
            $template->page_extra_metas["og:video:type"]   = "<meta property='og:video:type'   content='{$mitem->mime_type}'>";
            $template->page_extra_metas["og:video:width"]  = "<meta property='og:video:width'  content='{$mitem->get_item_width()}'>";
            $template->page_extra_metas["og:video:height"] = "<meta property='og:video:height' content='{$mitem->get_item_height()}'>";
        }
    }
}

if( ! empty($document->tag) ) $template->set("page_tag", $document->tag);

switch($document->layout)
{
    case "admin":
    case "embeddable":
        $dir_prefix = "contents/";
        break;
    default:
        $dir_prefix = "";
        break;
}
$template->page_contents_include = "{$dir_prefix}renderer.inc";
include "{$template->abspath}/{$document->layout}.php";
exit;
