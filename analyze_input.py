from gpt4all import GPT4All

model = GPT4All("gpt4all-falcon-newbpe-q4_0.gguf")

output = model.generate("What is 3+4?")

print(output)
