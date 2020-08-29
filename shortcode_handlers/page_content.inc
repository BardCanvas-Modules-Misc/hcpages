<?php
/**
 * Handler for page_content shortcode
 * 
 * @package    BardCanvas
 * @subpackage Hardcoded Pages
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * Trailing globals:
 * @var module[] $modules
 * @var module   $current_module Topmost module
 * @var module   $this_module    Self
 * @var config   $config
 * @var settings $settings
 * @var account  $account
 * 
 * Trailing vars:
 * @var array    $matches
 * @var string   $matched_str
 * @var string   $contents
 * @var array    $attributes
 * 
 * Expected attributes:
 * • path                         document path
 * • processed         true|false to show processed title or not, true by default
 * • include_autolinks true|false true by default
 * • class                        empty or css class list
 * • style                        empty or css styles
 */

use hng2_base\account;
use hng2_base\config;
use hng2_base\module;
use hng2_base\settings;
use hng2_modules\hcpages\document;

if( empty($attributes["path"]) )
{
    $contents = str_replace($matched_str, "", $contents);
    
    return;
}

$path = "{$config->datafiles_location}/hcpages/" . ltrim(trim(stripslashes($attributes["path"])), "/") . ".xml";

if( ! is_file($path) )
{
    $contents = str_replace($matched_str, "", $contents);
    
    return;
}

$document = new document($path);

if( $document->hidden )
{
    $contents = str_replace($matched_str, "", $contents);
    
    return;
}

$return = $attributes["processed"] != "false"
        ? $document->get_processed_content()
        : $document->content;
$return = "<div class='{$attributes["class"]}' style='{$attributes["style"]}'>$return</div>";
$contents = str_replace($matched_str, $return, $contents);
