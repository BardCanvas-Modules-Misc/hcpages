<?php
/**
 * Pages index
 *
 * @package    BardCanvas
 * @subpackage Hardcoded Pages
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 */

use hng2_modules\hcpages\document;

include "../config.php";
include "../includes/bootstrap.inc";

if( ! $account->_exists ) throw_fake_401();
if( $account->state != "enabled" ) throw_fake_401();

if( ! $account->has_admin_rights_to_module("hcpages") )
{
    $template->page_contents_include = "contents/request_login.inc";
    $template->set_page_title($current_module->language->index->title);
    include "{$template->abspath}/admin.php";
    die();
}

if( $_GET["mode"] == "create" )
{
    $document = new document();
    $template->page_contents_include = "contents/form.inc";
    $template->set_page_title($current_module->language->index->creating);
    include "{$template->abspath}/admin.php";
    die();
}

if( ! empty($_GET["edit"]) )
{
    $file = "{$config->datafiles_location}/hcpages/" . trim(stripslashes($_GET["edit"])) . ".xml";
    if( ! is_file($file) )
    {
        $error = $current_module->language->messages->file_not_found;
        $template->page_contents_include = "contents/index.inc";
        $template->set_page_title($current_module->language->index->title);
        include "{$template->abspath}/admin.php";
        die();
    }
    
    $document = new document();
    $document->set_from_file($file);
    $template->page_contents_include = "contents/form.inc";
    $template->set_page_title($current_module->language->index->editing);
    include "{$template->abspath}/admin.php";
    die();
}

if( ! empty($_GET["duplicate"]) )
{
    $file = "{$config->datafiles_location}/hcpages/" . trim(stripslashes($_GET["duplicate"])) . ".xml";
    if( ! is_file($file) )
    {
        $error = $current_module->language->messages->file_not_found;
        $template->page_contents_include = "contents/index.inc";
        $template->set_page_title($current_module->language->index->title);
        include "{$template->abspath}/admin.php";
        die();
    }
    
    $document = new document();
    $document->set_from_file($file);
    $document->path .= "-" . $current_module->language->copy;
    $document->title = "{$current_module->language->copy_of} {$document->title}";
    $template->page_contents_include = "contents/form.inc";
    $template->set_page_title($current_module->language->index->editing);
    include "{$template->abspath}/admin.php";
    die();
}

if( ! empty($_GET["delete"]) )
{
    $file = "{$config->datafiles_location}/hcpages/" . trim(stripslashes($_GET["delete"])) . ".xml";
    if( ! is_file($file) ) die($current_module->language->messages->file_not_found);
    if( ! @unlink($file) ) die($current_module->language->messages->cannot_delete_file);
    
    die("OK");
}

if( ! empty($_GET["toggle"]) )
{
    if( ! in_array($_GET["visibility"], explode(" ", "visible hidden")) )
        die($current_module->language->messages->invalid_visibility_flag);
    
    $file = "{$config->datafiles_location}/hcpages/" . trim(stripslashes($_GET["toggle"])) . ".xml";
    if( ! is_file($file) ) die($current_module->language->messages->file_not_found);
    
    $document = new document($file);
    
    if( $_GET["visibility"] == "hidden"  &&   $document->hidden ) die("OK");
    if( $_GET["visibility"] == "visible" && ! $document->hidden ) die("OK");
    
    $document->hidden = $_GET["visibility"] == "hidden" ? "true" : "";
    try
    {
        $document->save();
    }
    catch(\Exception $e)
    {
        die( $e->getMessage() );
    }
    
    die("OK");
}

#
# Save mode is AJAX called
#

if( $_GET["mode"] == "save" )
{
    header("Content-Type: text/plain; charset=utf-8");
    
    $path = "{$config->datafiles_location}/hcpages";
    if( ! is_dir($path) )
        if( ! @mkdir($path) )
            die($current_module->language->messages->cannot_create_path);
    
    $document = new document();
    $document->set_from_post();
    $document->path = trim($document->path, "/");
    
    if( empty($document->title) ) die($current_module->language->messages->missing_title);
    if( empty($document->content) ) die($current_module->language->messages->missing_content);
    
    if( ! in_array($document->layout, explode(" ", "main admin popup embeddable")) )
        die($current_module->language->messages->invalid_layout);
    
    if( empty($document->path)        ) $document->path = wp_sanitize_filename($document->title);
    if( empty($document->description) ) $document->description = make_excerpt_of($document->content, 200);
    
    $ext = end(explode(".", $document->path));
    if( ! empty($ext) )
    {
        if( strlen($ext) == 4 && $ext != "html" ) die($current_module->language->messages->extension_forbidden);
        if( strlen($ext) == 3 && $ext != "php"  ) die($current_module->language->messages->extension_forbidden);
    }
    
    if( stristr($document->path, "/") )
    {
        $path = "{$config->datafiles_location}/hcpages/" . dirname($document->path);
        if( ! is_dir($path) )
            if( ! @mkdir($path, 0777, true) )
                die($current_module->language->messages->cannot_create_doc_path);
    }
    
    $original_path = trim(stripslashes($_POST["original_path"]));
    if( empty($original_path) )
    {
        if( is_file("{$config->datafiles_location}/hcpages/{$document->path}.xml") )
            die($current_module->language->messages->file_exists);
        
        $document->creation_date = date("Y-m-d H:i:s");
        $document->created_by    = $account->id_account;
    }
    
    try
    {
        $document->save();
    }
    catch(\Exception $e)
    {
        die( $e->getMessage() );
    }
    
    $original_path = "{$config->datafiles_location}/hcpages/" . stripslashes($original_path);
    if( $document->path != $original_path && is_file($original_path) ) @unlink($original_path);
    
    die("OK");
}

#
# File browser
#

$template->page_contents_include = "contents/index.inc";
$template->set_page_title($current_module->language->index->title);
include "{$template->abspath}/admin.php";
