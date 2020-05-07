<?php
namespace hng2_modules\hcpages;

use hng2_repository\abstract_record;

class document extends abstract_record
{
    public $path;
    
    public $layout = "main";
    public $title;
    public $description;
    public $content;
    public $featured_image_path;
    public $tag;
    
    public $head_markuphead_markup;
    public $pre_content_markup;
    public $post_content_markup;
    
    public $creation_date;
    public $created_by;
    public $last_update;
    public $hidden;
    
    public function __construct($file = "")
    {
        if( ! empty($file) && is_file($file) ) $this->set_from_file($file);
    }
    
    public function set_new_id()
    {
        throw new \Exception("Method not implemented");
    }
    
    public function set_from_post()
    {
        foreach( $_POST as $key => $val )
            if( is_string($val) )
                $this->{$key} = trim(stripslashes($val));
    }
    
    public function set_from_file($file)
    {
        global $config;
        
        $xml  = simplexml_load_file($file);
        
        $path  = str_replace("{$config->datafiles_location}/hcpages/", "", $file);
        $parts = explode(".", $path); array_pop($parts);
        $path  = implode(".", $parts);
        
        $this->path                   = $path;
        $this->layout                 = trim($xml->layout                );
        $this->title                  = trim($xml->title                 );
        $this->description            = trim($xml->description           );
        $this->content                = trim($xml->content               );
        $this->featured_image_path    = trim($xml->featured_image_path   );
        $this->tag                    = trim($xml->tag                   );
        $this->head_markuphead_markup = trim($xml->head_markuphead_markup);
        $this->pre_content_markup     = trim($xml->pre_content_markup    );
        $this->post_content_markup    = trim($xml->post_content_markup   );
        $this->creation_date          = trim($xml->creation_date         );
        $this->created_by             = trim($xml->created_by            );
        $this->last_update            = trim($xml->last_update           );
        $this->hidden                 = trim($xml->hidden                );
        $this->last_update            = date("Y-m-d H:i:s", filemtime($file));
    }
    
    /**
     * @throws \Exception
     */
    public function save()
    {
        global $config, $modules;
        $current_module = $modules["hcpages"];
        
        $path = "{$config->datafiles_location}/hcpages/{$this->path}.xml";
        $path = preg_replace('/\.+/', ".", $path);
        
        $root = simplexml_load_string("<document />");
        $root->addChild("layout", $this->layout);
        add_cdata_node("title", $this->title, $root);
        add_cdata_node("description", $this->description, $root);
        add_cdata_node("content", $this->content, $root);
        $root->addChild("featured_image_path", $this->featured_image_path);
        $root->addChild("tag", $this->tag);
        add_cdata_node("head_markuphead_markup", $this->head_markuphead_markup, $root);
        add_cdata_node("pre_content_markup", $this->pre_content_markup, $root);
        add_cdata_node("post_content_markup", $this->post_content_markup, $root);
        $root->addChild("creation_date", $this->creation_date);
        $root->addChild("created_by", $this->created_by);
        $root->addChild("hidden", $this->hidden);
        
        $doc = new \DOMDocument();
        $doc->preserveWhiteSpace = false;
        $doc->formatOutput = true;
        $doc->loadXML( $root->asXML() );
        $xml = $doc->saveXML();
        
        if( ! @file_put_contents($path, $xml) )
            throw new \Exception(sprintf(
                $current_module->language->messages->cannot_save_file,
                $path
            ));
    }
    
    public function get_permalink($fully_qualified = false)
    {
        global $config;
        
        $prefix = $fully_qualified ? $config->full_root_url : $config->full_root_path;
        
        return "$prefix/$this->path";
    }
    
    public function get_processed_title($include_autolinks = true)
    {
        global $config;
        
        $contents = $this->title;
        $contents = convert_emojis($contents);
        if( $include_autolinks ) $contents = autolink_hash_tags($contents, "{$config->full_root_path}/tag/");
        
        return $contents;
    }
    
    public function get_processed_content()
    {
        global $config;
        
        $contents = $this->content;
        
        $contents = preg_replace(
            '@<p>(https?://([-\w\.]+[-\w])+(:\d+)?(/([\%\w/_\.#-]*(\?\S+)?[^\.\s])?)?)</p>@',
            '<p><a href="$1" target="_blank">$1</a></p>',
            $contents
        );
        
        $contents = convert_shortcodes($contents);
        $contents = convert_emojis($contents);
        $contents = convert_media_tags($contents);
        $contents = autolink_hash_tags($contents, "{$config->full_root_path}/tag/");
        
        return $contents;
    }
}
