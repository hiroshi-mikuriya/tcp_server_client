# frozen_string_literal: true

require 'socket'

Thread.new do
  TCPServer.open(12345) do |sock|
    client, = sock.accept
    puts client.recv(1024)
    client.puts 'hello world'
    client.close
  end
end

sleep(0.1)

TCPSocket.open('127.0.0.1', 12345) do |sock|
  sock.write 'goodbye'
  puts sock.gets
end
