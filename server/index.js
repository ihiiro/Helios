

const express = require('express')
const fs = require('node:fs')
const PORT = 4000
const app = express()



app.get('/api/slots', (req, res) => {
    console.log('received request')
    fs.readFile('./slots.json', (err, data) => {
        if (err)
            res.sendStatus(500)
        else
            res.send(data)
    })
})

app.listen(PORT, () => {
    console.log(`Helios server listening on ${PORT}`)
})
