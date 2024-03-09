# Not beautiful or clean code.

import xml.etree.ElementTree as ET
import os

def parse_doxygen_xml(xml_file):
    index = ET.parse(xml_file)
    root = index.getroot()

    for compound in root.findall('compound'):
        if compound.attrib['kind'] == "class":
            # print(compound.attrib['refid'])
            parse_class_xml(os.path.dirname(xml_file) + "/" + compound.attrib['refid'] + ".xml")

def parse_class_xml(xml_file):
    tree = ET.parse(xml_file)
    root = tree.getroot()

    markdown_content = []
    file_name = ""

    for compound in root.findall('compounddef'):
        kind = compound.attrib['kind']
        name = compound.find('compoundname').text
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
                        markdown_content.append(f"### {function.find('type').text} {function.find('definition').text}{function.find('argsstring').text}\n\n")
                        brief_description = function.find('briefdescription')
                        if (brief_description is not None):
                            text = element_to_text(brief_description).strip()
                            if text != "":
                                markdown_content.append(f"> {text}\n\n")

                        detailed_description = function.find('detaileddescription')
                        if (detailed_description is not None):
                            print(detailed_description)
                            for para in detailed_description.findall("para"):
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