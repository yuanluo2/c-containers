import shutil

def do_file_replace(filePath, replaceMap):
    with open(filePath, 'r', encoding='utf-8') as file:
        file_content = file.read()

    for pattern, target in replaceMap.items():
        file_content = file_content.replace(pattern, target)

    with open(filePath, 'w', encoding='utf-8') as file:
        file.write(file_content)


def create_array(targetFilePath, elementType, arrayTypeName):
    replaceMap = {
        '@ElementType': elementType,
        '@ArrayTypeName': arrayTypeName
    }

    templateFile = './template_array.txt'
    shutil.copyfile(templateFile, targetFilePath)
    do_file_replace(targetFilePath, replaceMap)


def create_doubly_linked_list(targetFilePath, elementType, listNodeTypeName, listTypeName):
    replaceMap = {
        '@ElementType': elementType,
        '@ListNodeTypeName': listNodeTypeName,
        '@ListTypeName': listTypeName
    }

    templateFile = './template_doubly_linked_list.txt'
    shutil.copyfile(templateFile, targetFilePath)
    do_file_replace(targetFilePath, replaceMap)


if __name__ == '__main__':
    create_array('array_int64.c', 'int64_t', 'ArrayInt64')
