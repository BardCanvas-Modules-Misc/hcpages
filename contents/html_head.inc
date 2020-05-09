<?php
/**
 * Hardcoded Pages HTML code injection
 *
 * @package    BardCanvas
 * @subpackage Hardcoded Pages
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * Imported globals:
 * @var document $document
 */

use hng2_modules\hcpages\document;

global $config, $document;

if( ! $config->globals["@hcpages:called_from_page_renderer"] ) return;
if( empty($document) ) return;
if( ! is_object($document) ) return;
if( ! $document instanceof document ) return;
if( empty($document->head_markup) ) return;
?>


<!-- Page HTML head markup -->

<?= $document->head_markup ?>


