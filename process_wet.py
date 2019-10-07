'''Process WET file containing crawling data.'''

import logging
import re

import click


logging.basicConfig(format='%(asctime)s - %(message)s')
logger = logging.getLogger(__name__)
logger.setLevel(logging.INFO)


class WETProcessor:
    def __init__(self, input_filename):
        self.input_filename = input_filename

        self.reader = WETReader(self.input_filename)
        self.parser = DocumentParser()

    def process(self):
        for document in self.reader.generate_documents():
            parsed_document = self.parser.parse_document(document)
            if parsed_document:
                formatted_document = self.format_document_for_output(parsed_document)
                print(formatted_document)

    def format_document_for_output(self, document):
        line = []
        line.append(document['uri'])
        line.extend(document['words'])
        return '\t'.join(line)


class DocumentParser:
    def parse_document(self, document):
        parsed_document =  {}

        if self.has_uri_header(document):
            parsed_document['uri'] = self.get_uri_from_document(document)
            parsed_document['words'] = self.parse_words_from_document(document)

        return parsed_document

    def has_uri_header(self, document):
        return 'WARC-Target-URI' in document

    def get_uri_from_document(self, document):
        return document['WARC-Target-URI']

    def parse_words_from_document(self, document):
        return self.parse_words_from_lines(document['Content'])

    def parse_words_from_lines(self, lines):
        words = []

        for line in lines:
            words.extend(self.parse_words_from_line(line))

        return words

    def parse_words_from_line(self, line):
        return re.findall(r'([\w\d]+)', line)


class WETReader:
    def __init__(self, filename):
        self.filename = filename

    def generate_documents(self):
        with open(self.filename) as file:
            while True:
                try:
                    document = self.read_document_from_file(file)

                    if not document:
                        continue

                    yield document
                except EOFError:
                    break

    def read_document_from_file(self, file):
        document = self.read_document_headers_from_file(file)
        document['Content'] = self.read_document_content_from_file(file)
        return document

    def read_document_headers_from_file(self, file):
        headers = {}

        for line in file:
            line = self.sanitize_line(line)
            headers.update(self.read_document_header_from_line(line))

            if self.has_read_all_headers(headers):
                break

        return headers

    def sanitize_line(self, line):
        return line.replace('\n', '')

    def read_document_header_from_line(self, line):
        header, value = self.split_header_line(line)
        value = self.cast_header_value(header, value)
        return {header: value}

    def split_header_line(self, line):
        header, *values = line.split(': ')
        return header, ''.join(values)

    def cast_header_value(self, header, value):
        if header == 'Content-Length':
            return int(value)
        return value

    def has_read_all_headers(self, headers):
        return 'Content-Length' in headers

    def read_document_content_from_file(self, file):
        content = []

        for line in file:
            if self.has_read_all_content(line):
                break

            content.append(self.read_document_content_from_line(line))

        return content

    def read_document_content_from_line(self, line):
        return self.sanitize_line(line)

    def has_read_all_content(self, line):
        return line == 'WARC/1.0\n'



@click.command(
    name='process_wet',
    short_help='process WET file containing crawling data',
    context_settings={'help_option_names': ['-h', '--help']})
@click.argument('input')
def main(input):
    processor = WETProcessor(input)
    processor.process()


if __name__ == '__main__':
    main()
