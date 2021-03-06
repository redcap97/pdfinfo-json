require 'test/unit'
require 'open3'

class PdfInfoJsonTest < Test::Unit::TestCase
  def test_pdf
    assert_command(
      %w(./pdfinfo-json test/data.pdf),
      expected_stdout: File.read('test/data-stdout.txt'),
    )
  end

  def test_blank_pdf
    assert_command(
      %w(./pdfinfo-json test/data-blank.pdf),
      expected_stdout: File.read('test/data-blank-stdout.txt'),
    )
  end

  def test_locked_pdf
    assert_command(
      %w(./pdfinfo-json test/data-locked.pdf),
      expected_stdout: File.read('test/data-locked-stdout.txt'),
      expected_stderr: File.read('test/data-locked-stderr.txt'),
      expected_status: 1
    )
  end

  def test_pdf_with_password
    assert_command(
      %w(./pdfinfo-json --owner-password 42 test/data-locked.pdf),
      expected_stdout: File.read('test/data-with-password-stdout.txt'),
    )
  end

  def test_pdf_with_annoations
    assert_command(
      %w(./pdfinfo-json test/data-with-annotations.pdf),
      expected_stdout: File.read('test/data-with-annotations-stdout.txt')
    )
  end

  def test_pdf_with_javascript
    assert_command(
      %w(./pdfinfo-json test/data-with-javascript.pdf),
      expected_stdout: File.read('test/data-with-javascript-stdout.txt')
    )
  end

  def test_pdf_with_form
    assert_command(
      %w(./pdfinfo-json test/data-with-form.pdf),
      expected_stdout: File.read('test/data-with-form-stdout.txt')
    )
  end

  def test_pdf_unembedded_fonts
    assert_command(
      %w(./pdfinfo-json test/data-unembedded-fonts.pdf),
      expected_stdout: File.read('test/data-unembedded-fonts-stdout.txt')
    )
  end

  def test_pdf_tagged
    assert_command(
      %w(./pdfinfo-json test/data-tagged.pdf),
      expected_stdout: File.read('test/data-tagged-stdout.txt')
    )
  end

  def test_pdf_with_image
    assert_command(
      %w(./pdfinfo-json test/data-with-image.pdf),
      expected_stdout: File.read('test/data-with-image-stdout.txt')
    )
  end

  def test_pdf_with_indexed_image
    assert_command(
      %w(./pdfinfo-json test/data-with-indexed-image.pdf),
      expected_stdout: File.read('test/data-with-indexed-image-stdout.txt')
    )
  end

  def test_pdf_with_stencil_image
    assert_command(
      %w(./pdfinfo-json test/data-with-stencil-image.pdf),
      expected_stdout: File.read('test/data-with-stencil-image-stdout.txt')
    )
  end

  private

  def assert_command(arguments, expected_stdout: '', expected_stderr: '', expected_status: 0)
    stdout, stderr, status = Open3.capture3(*arguments)

    assert_equal(expected_status, status.exitstatus)
    assert_equal(expected_stdout, stdout)
    assert_equal(expected_stderr, stderr)
  end
end
