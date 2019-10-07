'''Process WET file containing crawling data.'''

import logging
import re

import click


logging.basicConfig(format='%(asctime)s - %(message)s')
logger = logging.getLogger(__name__)
logger.setLevel(logging.INFO)


class WETProcessor:
    def process_wet_file(self, filename):
        logger.info(f'Processing {filename}')

        with open(filename) as file:
            self.process_file(file)

        logger.info(f'Finished processing {filename}')

    def process_file(self, file):
        while True:
            try:
                document = self.read_document_from_file(file)
                document['Words'] = self.parse_words_from_lines(document['Lines'])
            except EOFError:
                break

    def read_document_from_file(self, file):
        document = self.read_document_headers_from_file(file)
        document['Lines'] = self.read_document_content_from_file(file)
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

    def parse_words_from_lines(self, lines):
        words = []

        for line in lines:
            words.extend(self.parse_words_from_line(line))

        return words

    def parse_words_from_line(self, line):
        return re.findall(r'([\w\d]+)', line)


@click.command(
    name='process_wet',
    short_help='process WET file containing crawling data',
    context_settings={'help_option_names': ['-h', '--help']})
@click.argument('file')
def main(file):
    processor = WETProcessor()
    processor.process_wet_file(file)


if __name__ == '__main__':
    main()
