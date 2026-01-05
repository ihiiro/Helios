import '../styles/global.css'
import '../styles/App.css'
import { useState, useEffect } from 'react'
import down_arrow from '../assets/down_arrow.png'

function App() {
  const [ slot_active, set_slot_active ] = useState({})
  const [ slots, set_slots ] = useState({})
  const [ crosshair, set_crosshair ] = useState(-1)
  const [ can_save, set_can_save ] = useState(true)

  useEffect(() => {
    fetch('/get-slots').then(response => {
      if (response.ok) {
        response.json().then(slots => {
          set_slots(slots)
        })
      }
        
      else
        alert('Error fetching slots')
    })
  }, [])

  function toggle_slot(slot) { 
    set_slot_active({
      [slot]: !slot_active[slot]
    })
    set_crosshair((crosshair === slot) ? -1 : slot)
  }

  function modify_watch_context(target, key) {
    set_slots({
      ...slots,
      [key]: { ...slots[key], watch_context: target.value }
    })
  }

  function modify_backup_context(target, key) {
    set_slots({
      ...slots,
      [key]: { ...slots[key], backup_context: target.value }
    })
  }

  function remove_slot() {
    if ( crosshair > -1 ) {
      set_slots({
            ...slots,
            [crosshair]: { ...slots[crosshair], deleted: true }
          })
      set_crosshair(-1)
    }
    
  }

  function add_slot() {
    let ID = Math.random().toString().substr(2, 12)
    set_slots({
      ...slots,
      [ID]: { watch_context: 'new_slot', 
                      backup_context: 'new_slot', 
                      deleted: false }
    })
  }

  function save_all() {
    set_can_save(false)
    fetch('/post-slots',
      {
        method: 'POST',
        body: JSON.stringify(slots),
        headers: { 'content-type': 'application/json' }
      }
    ).then(() => set_can_save(true))
  }

  return (
    <>
    <div className='AO' >


      { Object.keys(slots).map( key => (
        !slots[key].deleted && <div className={`slot ${slot_active[key] ? 'slot-active' : ''}`} onClick={ () => toggle_slot(key) }>
          <label style={{color: '#009900'}}>Slot</label>
          <label>Watch this location for changes</label>
          <input value={slots[key].watch_context} onChange={e => modify_watch_context(e.target, key)} ></input> {/* directory to watch */}
          <img src={down_arrow} alt='down_arrow'></img>
          <label>Upon changes, create new copies in this location</label>
          <input value={slots[key].backup_context} onChange={e => modify_backup_context(e.target, key)} ></input> {/* backup directory */}
        </div>
      ) ) }

    </div>

    <div className='buttons'>
      <button style={{backgroundColor: ( can_save ) ? '#66b032' : '#ccc'}} disabled={!can_save} onClick={save_all}>Save all</button>
      <button style={{backgroundColor: '#00bfff'}} onClick={add_slot} >Add slot</button>
      <button style={{backgroundColor: '#cc0000'}} onClick={remove_slot}>Remove slot</button>
    </div>

    </>
  )
}

export default App;
