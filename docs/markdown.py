# Not beautiful or clean code.

import xml.etree.ElementTree as ET
import os
import re

def convert_function_name(function_name):
    result = ''
    for char in function_name:
        if char.isupper():
            result += '_' + char.lower()
        else:
            result += char

    # Remove leading underscore if present
    if result.startswith('_'):
        result = result[1:]

    return result

def extract_ref(input_string):
    pattern = r'godot::Ref<([^>]*)>'

    match = re.search(pattern, input_string)

    if match:
        content = match.group(1)
        return remove_namespace(content)

    return remove_namespace(input_string)

def replace_param(params):
    return params.replace("godot::Ref<", "").replace(">", "").replace("const ", "").replace("&", "").replace("godot::", "").replace("VCoreGDExt::", "")

def remove_namespace(name: str):
    last_index = name.rfind("::")

    if last_index != -1:
        second_element = name[last_index + 2:]
        return second_element

    return name

def parse_doxygen_xml(xml_file):
    index = ET.parse(xml_file)
    root = index.getroot()

    markdown_content = []
    markdown_content.append("# Classindex\n\n")

    for compound in root.findall('compound'):
        if compound.attrib['kind'] == "class":
            name = remove_namespace(compound.find('name').text)
            markdown_content.append(f"- [{name}]({name}.md)\n")
            parse_class_xml(os.path.dirname(xml_file) + "/" + compound.attrib['refid'] + ".xml")

    markdown_content.append("- [Examples](Examples.md)\n")
    write_markdown_file(markdown_content, "README.MD")

def parse_class_xml(xml_file):
    tree = ET.parse(xml_file)
    root = tree.getroot()

    markdown_content = []
    file_name = ""

    for compound in root.findall('compounddef'):
        kind = compound.attrib['kind']
        name = remove_namespace(compound.find('compoundname').text)
        file_name += name

        markdown_content.append(f"# {kind.capitalize()}: {name}\n\n")

        brief_description = compound.find('briefdescription')
        if (brief_description is not None):
            text = element_to_text(brief_description).strip()
            if text != "":
                markdown_content.append(f"> {text}\n\n")

        detailed_description = compound.find('detaileddescription')
        if (detailed_description is not None):
            text = element_to_text(detailed_description).strip()
            if text != "":
                markdown_content.append(f"> {text}\n\n")

        for section in compound.findall("sectiondef"):
            if section.attrib['kind'] == "public-func":
                for function in section.findall("memberdef"):
                    if function.find("type").text is not None:
                        ret_type = function.find('type').text
                        ref = function.find('type').find("ref")
                        if ref is not None:
                            if ret_type.find("<") == -1:
                                ret_type += ref.text
                            else:
                                ret_type += ref.text + ">"

                        markdown_content.append(f"### {extract_ref(ret_type)} {convert_function_name(remove_namespace(function.find('definition').text))}{replace_param(function.find('argsstring').text)}\n\n")
                        brief_description = function.find('briefdescription')
                        if (brief_description is not None):
                            text = element_to_text(brief_description).strip()
                            if text != "":
                                markdown_content.append(f"> {text}\n\n")

                        detailed_description = function.find('detaileddescription')
                        if (detailed_description is not None):
                            for para in detailed_description.findall("para"):
                                for parameterlist in para.findall("parameterlist"):
                                    for parameteritem in parameterlist.findall("parameteritem"):
                                        markdown_content.append(f"#### Param: {parameteritem.find('parameternamelist').find('parametername').text}\n")
                                        markdown_content.append(f"> {parameteritem.find('parameterdescription').find('para').text}\n")
                                for simplesect in para.findall("simplesect"):
                                    if simplesect.attrib["kind"] == "return":
                                        markdown_content.append(f"#### Return:\n\n")

                                        text = element_to_text(simplesect).strip()
                                        if text != "":
                                            markdown_content.append(f"> {text}\n\n")



    write_markdown_file(markdown_content, file_name.replace(":", "_") + ".md")
    # return markdown_content

def element_to_text(element):
    text = element.text or ''
    for child in element:
        text += element_to_text(child)
        if child.tail:
            text += child.tail
    return text

def write_markdown_file(markdown_content, output_file):
    with open(output_file, 'w') as f:
        for line in markdown_content:
            f.write(line)

if __name__ == "__main__":
    index_file = "xml/index.xml"
    markdown_content = parse_doxygen_xml(index_file)