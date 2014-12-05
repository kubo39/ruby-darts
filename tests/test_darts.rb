require '../ext/darts'
require 'minitest/unit'
require 'minitest/autorun'

class TestDarts < MiniTest::Test
  def setup
    super
    datapath = File.expand_path("../", __FILE__) + '/mock_data.darts'
    @ins = Darts::Trie.new(datapath)
  end

  def test_common_prefix_search
    assert_equal(['star'], @ins.common_prefix_search('star'))
    assert_equal(['star', 'star wars'], @ins.common_prefix_search('star wars'))
  end

  def test_longest_match
    assert_equal('star', @ins.longest_match('star'))
    assert_equal('star wars', @ins.longest_match('star wars'))
  end
end
