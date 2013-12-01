<?php
private function initParser(){
	$this->parser = xml_parser_create();
	xml_set_object ($this->parser, $this);
	xml_parser_set_option($this->parser, XML_OPTION_CASE_FOLDING, 1);
	xml_parser_set_option($this->parser, XML_OPTION_TARGET_ENCODING, 'UTF-8');
	xml_set_element_handler($this->parser, 'startElement', 'endElement');
	xml_set_character_data_handler($this->parser, 'characterData');
	xml_set_default_handler($this->parser, 'defaultHandler');
}
?>