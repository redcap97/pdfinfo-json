require 'test/unit'
require 'open3'

class PdfInfoJsonTest < Test::Unit::TestCase
  def test_pdf
    assert_command(
      %w(./pdfinfo-json test/data.pdf),
      expected_stdout: File.read('test/data-stdout.txt'),
    )
  end

  private

  def assert_command(arguments, expected_stdout: '', expected_stderr: '', expected_status: 0)
    stdout, stderr, status = Open3.capture3(*arguments)

    assert_equal(expected_status, status.to_i)
    assert_equal(expected_stdout, stdout)
    assert_equal(expected_stderr, stderr)
  end
end
