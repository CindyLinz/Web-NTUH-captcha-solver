Module = {
  onRuntimeInitialized: () => {
    Module.read = Module.cwrap('read', 'string', ['array'])
    Module.add_symbol = Module.cwrap('add_symbol', 'number', ['number', 'number', 'number', 'array'])

    let symbols = "123456789BDFHJLNPRTVXZ"
    let symbol_waiting = symbols.length
    for(let i=0; i<symbols.length; ++i){
      let img = new Image
      let symbol_name = symbols[i]
      img.onload = () => {
        let canvas = document.createElement('canvas')
        canvas.width = img.width
        canvas.height = img.height
        let ctx = canvas.getContext('2d')
        ctx.drawImage(img, 0, 0)
        let data = ctx.getImageData(0, 0, img.width, img.height)
        Module.add_symbol(symbol_name.charCodeAt(0), img.width, img.height, data.data)
        if( !--symbol_waiting ){
          document.querySelector('#panel').style.display = ''
          document.querySelector('#upload').addEventListener('change', (ev) => {
            if( ev.target.files[0] ){
              let img = new Image
              img.onload = () => {
                document.querySelectorAll('img').forEach((elem) => {
                  elem.parentNode.removeChild(elem)
                })
                document.body.append(img)
                let canvas = document.createElement('canvas')
                canvas.width = img.width
                canvas.height = img.height
                let ctx = canvas.getContext('2d')
                ctx.drawImage(img, 0, 0)
                let data = ctx.getImageData(0, 0, img.width, img.height)

                let res = Module.read(data.data)
                document.querySelector('#result').value = res
              }
              img.src = URL.createObjectURL(ev.target.files[0])

            }
          })
        }
      }
      img.src = 'symbol/'+symbol_name+'.png'
    }
  }
}
